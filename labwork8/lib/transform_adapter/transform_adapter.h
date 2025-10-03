#pragma once

#include "../all_includes.h"


template<class Iterator, typename Predicate>
class TransformAdapter {

public:

    TransformAdapter(Iterator first, Iterator last, Predicate predicate) 
    : begin_(first), end_(last), predicate_(predicate) {}

    class iterator {
        
    public:
            
        using iterator_category = std::input_iterator_tag;
        using value_type = std::invoke_result_t<Predicate, typename std::iterator_traits<Iterator>::value_type>;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = const value_type*;
        using reference = const value_type&;
    
        iterator() : current_(), end_(), predicate_(), current_value_() {}
    
        iterator(Iterator current, Iterator end, Predicate predicate)
        : current_(current), end_(end), predicate_(predicate), current_value_() {
            if (current_ != end_) {
                current_value_ = predicate_(*current_); 
            }
        }
    
        reference operator*() const {
            return current_value_;
        }

        pointer operator->() const {
            return &current_value_;
        }
    
        iterator& operator++() {
            ++current_;  
            if (current_ != end_) {
                current_value_ = predicate_(*current_); 
            }
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

        Iterator current_;
        Iterator end_;
        value_type current_value_;
        Predicate predicate_;

    };

    iterator begin() const { return iterator(begin_, end_, predicate_); }

    iterator end() const { return iterator(end_, end_, predicate_); }

private:

    Iterator begin_;
    Iterator end_;
    Predicate predicate_;

};


template<typename Predicate>
class Transform {

public:

    Transform(Predicate predicate) : predicate_(predicate) {}

    Predicate get_pred() const { return predicate_; };

private:

    Predicate predicate_;

};


template <class Flow, typename Predicate>
auto operator|(Flow&& flow, Transform<Predicate>&& transform) {
    return TransformAdapter<typename Flow::iterator, Predicate>(flow.begin(), flow.end(), transform.get_pred());
}
