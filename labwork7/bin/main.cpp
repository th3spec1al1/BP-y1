#include <iostream>
#include <limits>
#include <unrolled_list.h>

template <typename T>
class Sorted {
public:
    Sorted(const T& obj) : a(obj) {}

    class iterator {
    public:
        using value_type = typename T::value_type;
        using reference = const value_type&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;

        iterator(const T* obj, bool flag = false) : a(obj), flag(flag), count(0) {
            if (!flag) {
                search_min();
            }
        }

        iterator& operator++() {
            if (++count >= current_min_count) {
                search_next();
                count = 0;
            }
            return *this;
        }

        bool operator==(const iterator& other) const {
            return flag == other.flag;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        reference operator*() const {
            return min;
        }

    private:
        void search_min() {
            auto it = a->begin();
            if (it == a->end()) {
                flag = true;
                return;
            }

            min = *it;
            current_min_count = 1;

            for (++it; it != a->end(); ++it) {
                if (*it < min) {
                    min = *it;
                    current_min_count = 1;
                } else if (*it == min) {
                    current_min_count++;
                }
            }
        }

        void search_next() {
            value_type next_min = std::numeric_limits<value_type>::max();
            size_t next_count = 0;
            bool found = false;

            for (auto it = a->begin(); it != a->end(); ++it) {
                if (*it > min) {
                    if (*it < next_min) {
                        next_min = *it;
                        next_count = 1;
                        found = true;
                    } 
                    else if (*it == next_min) {
                        next_count++;
                    }
                }
            }

            if (found) {
                min = next_min;
                current_min_count = next_count;
            } 
            else {
                flag = true;
            }
        }

        const T* a;
        value_type min;
        size_t current_min_count;
        size_t count;
        bool flag;
    };

    iterator begin() const {
        return iterator(&a);
    }

    iterator end() const {
        return iterator(&a, true);
    }

private:
    const T& a;
};

int main(int argc, char** argv) {
    unrolled_list<int> a;
    a.push_back(5);
    a.push_back(3);
    a.push_back(4);
    a.push_back(1);
    a.push_back(2);

    Sorted b(a);

    for (auto j : a) {
        std::cout << j << " ";
    }
    std::cout << "\n";

    for (auto j : b) {
        std::cout << j << " ";
    }
    std::cout << "\n";
    std::cout << "\n";

    a.clear();

    a.push_back(1);
    a.push_back(2);
    a.push_back(1);
    a.push_back(1);
    a.push_back(2);

    Sorted c(a);

    for (auto j : a) {
        std::cout << j << " ";
    }
    std::cout << "\n";

    for (auto j : c) {
        std::cout << j << " ";
    }
    std::cout << "\n";

    return 0;
}
