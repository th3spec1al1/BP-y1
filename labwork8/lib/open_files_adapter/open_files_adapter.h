#pragma once

#include "../all_includes.h"


template <class Iterator>
class OpenFilesAdapter {

public:

    OpenFilesAdapter(Iterator first, Iterator last) : begin_(first), end_(last) {}

    class iterator {

    public:

        using iterator_category = std::input_iterator_tag;
        using value_type = std::ifstream;
        using difference_type = std::ptrdiff_t;
        using pointer = std::ifstream*;
        using reference = std::ifstream&;

        iterator() : current_iterator_(), end_iterator_(), current_file_(nullptr) {}

        iterator(Iterator first, Iterator last) 
        : current_iterator_(first), end_iterator_(last), current_file_(nullptr){
            open_next_file();
        }

        reference operator*() const {
            if (current_iterator_ == end_iterator_) {
                throw std::runtime_error("It is end iterator and operator* can't use, because current_file = nullptr");
            }
            return *current_file_;
        }

        pointer operator->() const {
            return current_file_;
        }

        iterator& operator++() {
            if (current_file_) {
                current_file_->close();
                delete current_file_;
                current_file_ = nullptr;
            }

            if (current_iterator_ == end_iterator_) {
                return *this;
            }

            ++current_iterator_;
            open_next_file();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            if (current_iterator_ != end_iterator_) {
                ++(*this);
            }
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return current_iterator_ == other.current_iterator_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        ~iterator() {
            if (current_file_) {
                current_file_->close();
                delete current_file_;
                current_file_ = nullptr;
            }
        }

    private:

        Iterator current_iterator_;
        Iterator end_iterator_;
        pointer current_file_;

        void open_next_file() {
            if (current_iterator_ != end_iterator_) {
                std::string file_path_ = *current_iterator_;
                current_file_ = new std::ifstream(file_path_);

                if (!current_file_->is_open()) {
                    std::cerr << "Error opening file: " << file_path_ << std::endl;
                    delete current_file_;
                    current_file_ = nullptr;
                }
            } 
            else {
                current_file_ = nullptr;
            }
        }

    };

    iterator begin() const { return iterator(begin_, end_); };

    iterator end() const { return iterator(); };

private:

    const Iterator begin_;
    const Iterator end_;

};


class OpenFiles {};


template <class Flow>
auto operator|(Flow&& flow, OpenFiles&&) {
    return OpenFilesAdapter<typename Flow::iterator>(flow.begin(), flow.end());
}