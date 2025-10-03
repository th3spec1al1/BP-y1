#pragma once

#include "../all_includes.h"


template<typename Iterator, typename Predicate>
class SplitExpectedAdapter {

public:

    using InputType = typename std::iterator_traits<Iterator>::value_type;
    using ValueType = typename InputType::value_type; 
    using ErrorType = typename InputType::error_type; 

private:

    Iterator begin_;
    Iterator end_;
    Predicate predicate_;

public:

    SplitExpectedAdapter(Iterator first, Iterator last, Predicate predicate)
        : begin_(first), end_(last), predicate_(predicate) {}

    class ExpectedFlow {
    public:
        using value_type = ValueType;

        ExpectedFlow(Iterator begin, Iterator end, Predicate func) : begin_(begin), end_(end), func_(func) {}

        class iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = ValueType;
            using difference_type = std::ptrdiff_t;
            using pointer = const value_type*;
            using reference = const value_type&;

            iterator() = default;

            iterator(Iterator current, Iterator end, Predicate func) : current_(current), end_(end), func_(func) {
                advance_to_next_valid();
            }

            reference operator*() const { return current_->value(); }

            pointer operator->() const { return &current_->value(); }

            iterator& operator++() {
                ++current_;
                advance_to_next_valid();
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const { return current_ == other.current_; }
            bool operator!=(const iterator& other) const { return !(*this == other); }

        private:
            void advance_to_next_valid() {
                while (current_ != end_ && !current_->has_value()) {
                    ++current_;
                }
            }

            Iterator current_;
            Iterator end_;
            Predicate func_;
        };

        iterator begin() const { return iterator(begin_, end_, func_); };
        iterator end() const { return iterator(end_, end_, func_); };

    private:
        Iterator begin_;
        Iterator end_;
        Predicate func_;
    };

    class UnexpectedFlow {
    public:
        using value_type = ErrorType;

        UnexpectedFlow(Iterator begin, Iterator end, Predicate func) : begin_(begin), end_(end), func_(func) {}

        class iterator {

        public:

            using iterator_category = std::input_iterator_tag;
            using value_type = ErrorType;
            using difference_type = std::ptrdiff_t;
            using pointer = const value_type*;
            using reference = const value_type&;

            iterator() = default;

            iterator(Iterator current, Iterator end, Predicate func) : current_(current), end_(end), func_(func) {
                advance_to_next_invalid();
            }

            reference operator*() const { return current_->error(); }

            pointer operator->() const { return &current_->error(); }

            iterator& operator++() {
                ++current_;
                advance_to_next_invalid();
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator& other) const { return current_ == other.current_; }
            bool operator!=(const iterator& other) const { return !(*this == other); }

        private:

            void advance_to_next_invalid() {
                while (current_ != end_ && current_->has_value()) {
                    ++current_;
                }
            }

            Iterator current_;
            Iterator end_;
            Predicate func_;
        };

        iterator begin() const { return iterator(begin_, end_, func_); };
        iterator end() const { return iterator(end_, end_, func_); };

    private:
        Iterator begin_;
        Iterator end_;
        Predicate func_;
    };


    auto GetExpectedFlow() const {
        return ExpectedFlow(begin_, end_, predicate_);
    }

    auto GetUnexpectedFlow() const {
        return UnexpectedFlow(begin_, end_, predicate_);
    }

};


template<typename Predicate>
class SplitExpected {

public:

    SplitExpected(Predicate predicate) : predicate_(predicate) {}

    Predicate get_pred() const { return predicate_; };

private:

    Predicate predicate_;

};


template <class Flow, typename Predicate>
auto operator|(Flow&& flow, SplitExpected<Predicate>&& transform) {
    SplitExpectedAdapter<typename Flow::iterator, Predicate> result(flow.begin(), flow.end(), transform.get_pred());
    return std::make_pair(result.GetExpectedFlow(), result.GetUnexpectedFlow());
}
