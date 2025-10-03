#pragma once

#include "../all_includes.h"


template <typename Iterator>
class SplitAdapter {

public:

    using value_type = std::string;

    SplitAdapter(Iterator begin, Iterator end, std::string delimiters) 
    : begin_(begin), end_(end), delimiters_(delimiters) {}

    class iterator {

    public:

        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        iterator(Iterator first, Iterator last, std::string delimiters) 
        : current_iterator_(first), end_iterator_(last), dels_(delimiters), current_value_("") {
            buffer_.clear();
            split_next();
        }

        iterator(const iterator& other) : current_iterator_(other.current_iterator_), end_iterator_(other.end_iterator_),
        dels_(other.dels_), current_value_(other.current_value_), buffer_(other.buffer_) {}

        reference operator*() const { return current_value_; }

        pointer operator->() const { return &current_value_; }

        iterator& operator++() {
            split_next();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            if (current_iterator_ == end_iterator_ && other.current_iterator_ == end_iterator_) {
                return current_value_.empty() == other.current_value_.empty(); 
            }
            return current_iterator_ == other.current_iterator_ && buffer_ == other.buffer_ && current_value_ == other.current_value_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:

        Iterator current_iterator_;
        Iterator end_iterator_;
        std::string dels_;
        std::string buffer_;
        std::string current_value_;

        void fill_buffer() {
            if (current_iterator_ != end_iterator_) {
                std::stringstream ss;
                ss << current_iterator_->rdbuf();
                buffer_ = ss.str();
            } else {
                buffer_.clear();
            }
        }

        void split_next() {
            current_value_.clear();

            if (current_iterator_ == end_iterator_ && buffer_.empty()) {
                return; 
            }

            if (buffer_.empty()) {
                fill_buffer();
                if (buffer_.empty()) {
                    return; 
                }
            }

            size_t pos = buffer_.find(dels_.front());
            if (pos != std::string::npos) {
                current_value_ = buffer_.substr(0, pos);
                buffer_.erase(0, pos+1); 
            } else {
                current_value_ = buffer_;
                buffer_.clear();
                ++current_iterator_;
                fill_buffer();
            }
        }
    };

    iterator begin() const { return iterator(begin_, end_, delimiters_); }
    iterator end() const { return iterator(end_, end_, delimiters_); }

private:

    Iterator begin_;
    Iterator end_;
    std::string delimiters_;

};


class Split {

public:

    Split(std::string&& delimiters) : delimiters_(delimiters) {}

    std::string get_del() const { return delimiters_; }

private:

    std::string delimiters_;

};


template<class Flow>
auto operator|(Flow&& flow, Split&& split) {
    return SplitAdapter<typename Flow::iterator>(flow.begin(), flow.end(), split.get_del());
}