#pragma once

#include "../all_includes.h"


template <typename Iterator>
class DropNulloptAdapter {

public:

    using optional_type = typename std::iterator_traits<Iterator>::value_type; 
    using value_type = typename optional_type::value_type; 
    using reference = const value_type&;
    using pointer = const value_type*;

    DropNulloptAdapter(Iterator begin, Iterator end) : begin_(begin), end_(end) {}

    class iterator {

    public:

        using iterator_category = std::input_iterator_tag;
        using value_type = typename DropNulloptAdapter::value_type;
        using difference_type = std::ptrdiff_t;
        using pointer = DropNulloptAdapter::pointer;
        using reference = DropNulloptAdapter::reference;

        iterator(Iterator current, Iterator end) : current_(current), end_(end) {
            find_next();
        }

        reference operator*() const { return current_->value(); }

        pointer operator->() const { return &(current_->value()); } 

        iterator& operator++() {
            ++current_;
            find_next();
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

        void find_next() {
            while (current_ != end_ && !current_->has_value()) {
                ++current_;
            }
        }

        Iterator current_;
        Iterator end_;

    };

    iterator begin() const { return iterator(begin_, end_); }

    iterator end() const { return iterator(end_, end_); }

private:

    Iterator begin_;
    Iterator end_;

};


class DropNullopt {};


template <class Flow>
auto operator|(Flow&& flow, DropNullopt&&) {
    return DropNulloptAdapter<typename Flow::iterator>(flow.begin(), flow.end());
}