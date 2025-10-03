#pragma once

#include "dir_adapter/dir_adapter.h"
#include "open_files_adapter/open_files_adapter.h"
#include "filter_adapter/filter_adapter.h"
#include "out_adapter/out_adapter.h"
#include "as_data_flow_adapter/as_data_flow_adapter.h"
#include "as_vector_adapter/as_vector_adapter.h"
#include "split_adapter/split_adapter.h"
#include "write_adapter/write_adapter.h"
#include "transform_adapter/transform_adapter.h"
#include "dropnullopt_adapter/dropnullopt_adapter.h"
#include "split_expected_adapter/split_expected_adapter.h"

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;

	bool operator==(const KV& other) const = default;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

	bool operator==(const JoinResult& other) const = default;
};




template <typename Value, typename Aggregator, typename KeyExtractor>
class AggregateByKey {

private:

    Value value_;
    Aggregator aggregator_;
    KeyExtractor key_extractor_;

public:

    AggregateByKey(Value value, Aggregator aggregator, KeyExtractor key_extractor)
    : value_(value), aggregator_(aggregator), key_extractor_(key_extractor) {}

    template <typename Input>
    class Iterator {

    private:

        using BaseIterator = decltype(std::begin(std::declval<Input>()));
        BaseIterator it_;
        BaseIterator end_;
        using KeyType = decltype(std::declval<KeyExtractor>()(*std::begin(std::declval<Input>())));
        std::unordered_map<KeyType, Value> aggregated_;
        typename std::unordered_map<KeyType, Value>::iterator current_;
        bool initialized_ = false;

        Value value_;
        Aggregator aggregator_;
        KeyExtractor key_extractor_;

        void initialize() {
            if (initialized_) {
                return;
            }
            
            while (it_ != end_) {
                auto&& item = *it_;
                KeyType key = key_extractor_(item);
                auto [iter, inserted] = aggregated_.try_emplace(key, value_);
                aggregator_(iter->second, item);
                ++it_;
            }
            current_ = aggregated_.begin();
            initialized_ = true;
        }

    public:

        Iterator(BaseIterator start, BaseIterator end, Value value, Aggregator aggregator, KeyExtractor key_extractor)
        : it_(start), end_(end), value_(value), 
              aggregator_(aggregator), key_extractor_(key_extractor) {
            initialize();
        }

        Iterator& operator++() {
            initialize();
            ++current_;
            return *this;
        }

        auto operator*() {
            initialize();
            return KV<KeyType, Value>{current_->first, current_->second};
        }

        bool operator!=(const Iterator& other) const {
            if (!initialized_ || !other.initialized_) {
                return it_ != other.it_;
            }
            return current_ != other.current_;
        }

    };

    template <typename Input>
    class Aggregated { 

    private:

        Input input_;
        AggregateByKey<Value, Aggregator, KeyExtractor> aggregator_;
    
    public:

        Aggregated(Input input, AggregateByKey<Value, Aggregator, KeyExtractor> aggregator)
        : input_(input), aggregator_(aggregator) {}

        auto begin() {
            return Iterator<Input>(input_.begin(), input_.end(), aggregator_.value_, 
             aggregator_.aggregator_, aggregator_.key_extractor_);
        }
        
        auto end() {
            return Iterator<Input>(input_.end(), input_.end(), aggregator_.value_,
             aggregator_.aggregator_, aggregator_.key_extractor_);
        }

    };

    template <typename Flow>
    friend auto operator|(Flow&& input, AggregateByKey&& obj) {
        return Aggregated<Flow>(input, obj);
    }
};






template <typename RightFlow>
class JoinAdapter1 {

private:

    RightFlow right_flow_;

public:

    explicit JoinAdapter1(RightFlow right_flow) : right_flow_(right_flow) {}

    template <typename LeftFlow>
    friend auto operator|(LeftFlow&& left_flow, JoinAdapter1&& obj) {
        using LeftKeyValue = std::decay_t<decltype(*std::begin(left_flow))>;
        using RightKeyValue = std::decay_t<decltype(*std::begin(right_flow_))>;
        using Key = decltype(std::declval<LeftKeyValue>().key);
        using LeftValue = decltype(std::declval<LeftKeyValue>().value);
        using RightValue = decltype(std::declval<RightKeyValue>().value);

        std::unordered_map<Key, std::vector<RightValue>> right_lookup;
        for (const auto& it : obj.right_flow_) {
            right_lookup[it.key].push_back(it.value);
        }

        std::vector<JoinResult<LeftValue, RightValue>> result;
        for (const auto& left_item : left_flow) {
            auto it = right_lookup.find(left_item.key);
            if (it != right_lookup.end()) {
                for (const auto& right_val : it->second) {
                    result.push_back({left_item.value, right_val});
                }
            } else {
                result.push_back({left_item.value, std::nullopt});
            }
        }
        return result;
    }

};

template <typename RightFlow>
auto Join(RightFlow&& right_flow) {
    return JoinAdapter1<RightFlow>(right_flow);
}

template <typename RightFlow, typename GetLeftKey, typename GetRightKey>
class JoinAdapter2 {

private:

    RightFlow right_flow_;
    GetLeftKey get_left_key_;
    GetRightKey get_right_key_;

public:

    explicit JoinAdapter2(RightFlow right_flow, GetLeftKey get_left_key, GetRightKey get_right_key)
    : right_flow_(right_flow), get_left_key_(get_left_key), get_right_key_(get_right_key) {}

    template <typename LeftFlow>
    friend auto operator|(LeftFlow&& left_flow, JoinAdapter2&& obj) {
        using LeftValue = std::decay_t<decltype(*std::begin(left_flow))>;
        using RightValue = std::decay_t<decltype(*std::begin(right_flow_))>;
        using Key = decltype(get_left_key_(std::declval<LeftValue>()));

        std::unordered_map<Key, std::vector<RightValue>> right_lookup;
        for (const auto& it : obj.right_flow_) {
            right_lookup[obj.get_right_key_(it)].push_back(it);
        }

        std::vector<JoinResult<LeftValue, RightValue>> result;
        for (const auto& left_item : left_flow) {
            Key key = obj.get_left_key_(left_item);
            auto it = right_lookup.find(key);
            if (it != right_lookup.end()) {
                for (const auto& right_val : it->second) {
                    result.push_back({left_item, right_val});
                }
            } else {
                result.push_back({left_item, std::nullopt});
            }
        }
        return result;
    }
    
};

template <typename RightFlow, typename GetLeftKey, typename GetRightKey>
auto Join(RightFlow&& right_flow, GetLeftKey get_left_key, GetRightKey get_right_key) {
    return JoinAdapter2<RightFlow, GetLeftKey, GetRightKey>(right_flow, get_left_key, get_right_key);
}