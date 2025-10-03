#pragma once

#include "../all_includes.h"


template <class Container>
class AsDataFlow {

public:

    AsDataFlow(const Container& container) : container_(container) {}

    class iterator {

    public:
    
        using iterator_category = std::input_iterator_tag;
        using iterator_type = decltype(std::begin(std::declval<Container>()));
        using value_type = typename std::iterator_traits<iterator_type>::value_type;
        using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
        using pointer = typename std::iterator_traits<iterator_type>::pointer;
        using reference = typename std::iterator_traits<iterator_type>::reference;

        iterator() : current_() {}

        iterator(iterator_type current, iterator_type end) : current_(current), end_(end) {}

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

        iterator_type current_;
        iterator_type end_;

    };

    iterator begin() const { return iterator(container_.begin(), container_.end()); }

    iterator end() const { return iterator(container_.end(), container_.end()); }

    using value_type = typename iterator::value_type;

private:

    const Container& container_;

};