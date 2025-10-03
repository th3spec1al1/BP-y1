#pragma once

#include "../all_includes.h"


class Dir {

public:

    Dir (const std::string& path, bool flag = true) : root_directory_(path), recursive_(true) {}

    class iterator {

    public:

        using iterator_category = std::input_iterator_tag;
        using value_type = std::string;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        iterator() {}

        iterator(const std::string& path_) : root_dir_(path_) {
            directory_iterator_ = std::filesystem::recursive_directory_iterator(root_dir_);
            find_next_file();
        }

        reference operator*() const {
            return current_file_;
        }

        pointer operator->() const {
            return &current_file_;
        }

        iterator& operator++() {
            if (directory_iterator_ != std::filesystem::recursive_directory_iterator()) {
                ++directory_iterator_;
                find_next_file();
            }
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            if (directory_iterator_ != std::filesystem::recursive_directory_iterator()) {
                ++(*this);
            }
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return directory_iterator_ == other.directory_iterator_;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

    private:

        std::filesystem::recursive_directory_iterator directory_iterator_;
        value_type root_dir_;
        value_type current_file_; 

        void find_next_file() {
            while (directory_iterator_ != std::filesystem::recursive_directory_iterator()) {
                if (std::filesystem::is_regular_file(*directory_iterator_)) {
                    current_file_ = directory_iterator_->path().string();
                    return;
                }

                ++directory_iterator_;
            }
        }

    };

    iterator begin() const { return iterator(root_directory_); }

    iterator end() const { return iterator(); }

private:

    std::string root_directory_;
    bool recursive_ = false; 

};