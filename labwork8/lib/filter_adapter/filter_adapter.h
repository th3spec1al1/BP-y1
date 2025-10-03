#pragma once

#include "../all_includes.h"


template<class Iterator, typename Predicate>
class FilterAdapter {

public:

    FilterAdapter(Iterator first, Iterator last, Predicate predicate) :begin_(first), end_(last), predicate_(predicate) {}

    class iterator {
        
    public:
            
        using iterator_category = std::input_iterator_tag;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using pointer = typename std::iterator_traits<Iterator>::pointer;
        using reference = typename std::iterator_traits<Iterator>::reference;
    
        iterator() : current_(), end_(), predicate_() {}
    
        iterator(Iterator current, Iterator end, Predicate predicate)
            : current_(current), end_(end), predicate_(predicate) {
            find_next_valid();
        }
    
        reference operator*() const {
            return *current_;
        }

        pointer operator->() const {
            return &(*current_);
        }
    
        iterator& operator++() {
            if (current_ != end_) { 
                ++current_;
            }
            find_next_valid();
            return *this;
        }
    
        iterator operator++(int) {
            iterator tmp = *this;
            if (current_ != end_) {
                ++(*this);
            }
            return tmp;
        }
    
        bool operator==(const iterator& other) const { return current_ == other.current_; }

        bool operator!=(const iterator& other) const { return !(*this == other); }
    
    private:

        Iterator current_;
        Iterator end_;
        Predicate predicate_;

        void find_next_valid() {
            while (current_ != end_ && !predicate_(*current_)) {
                ++current_;
            }
        }

    };

    iterator begin() const { return iterator(begin_, end_, predicate_); }

    iterator end() const { return iterator(end_, end_, predicate_); }

private:

    Iterator begin_;
    Iterator end_;
    Predicate predicate_;

};


template<typename Predicate>
class Filter {

public:

    Filter(Predicate predicate) : predicate_(predicate) {}

    Predicate get_pred() const { return predicate_; };

private:

    Predicate predicate_;

};


template <class Flow, typename Predicate>
auto operator|(Flow&& flow, Filter<Predicate>&& filter) {
    return FilterAdapter<typename Flow::iterator, Predicate>(flow.begin(), flow.end(), filter.get_pred());
}
