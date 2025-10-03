#include <iostream>
#include <memory>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <initializer_list>

template<typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {

public:

    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

private:

    struct Node {
        pointer data_;
        size_type data_size_;
        Node* next_;
        Node* prev_;

        Node()
            : data_(static_cast<pointer>(operator new(NodeMaxSize * sizeof(T)))), // ((pointer)new char[NodeMaxSize * sizeof(T)])
            data_size_(0), next_(nullptr), prev_(nullptr) {} 

        ~Node() {
            for (size_type i = 0; i < data_size_; ++i) {
                (&data_[i])->~T();
            }

            operator delete(data_);
            data_ = nullptr;
        }
    };

    using alloc_traits = typename std::allocator_traits<Allocator>;
    using node_allocator_type = typename alloc_traits::template rebind_alloc<Node>;

    node_allocator_type node_allocator_;

    Node* allocate_node() {
        Node* new_node = std::allocator_traits<node_allocator_type>::allocate(node_allocator_, 1);
        
        try {
            new (new_node) Node();
        } catch (...) {
            std::allocator_traits<node_allocator_type>::deallocate(node_allocator_, new_node, 1);
            throw;
        }

        return new_node;
    }

    void deallocate_node(Node *p) {
        p->~Node();
        std::allocator_traits<node_allocator_type>::deallocate(node_allocator_, p, 1);
    }

    size_type size_ = 0;
    Node* head_ = nullptr;
    Node* tail_ = nullptr;

public:

    unrolled_list(const allocator_type& allocator_ = allocator_type()) 
        : node_allocator_(allocator_), size_(0), head_(nullptr), tail_(nullptr) {}

    unrolled_list(const unrolled_list &other) : node_allocator_(other.node_allocator_) {
        try {
            for (const auto &value: other) {
                push_back(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    template<typename InputIterator>
    unrolled_list(InputIterator first, InputIterator last, const allocator_type &allocator = allocator_type())
        : node_allocator_(allocator) {
        try {
            while (first != last) {
                push_back(*first);
                ++first;
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    unrolled_list(size_type k, const_reference value, const allocator_type &allocator = allocator_type())
            : node_allocator_(allocator) {
        try {
            for (size_type i = 0; i < k; ++i) {
                push_back(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    unrolled_list(std::initializer_list<value_type> obj, const allocator_type &allocator = allocator_type())
        : node_allocator_(allocator) {
        try {
            for (const auto& value : obj) {
                push_back(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    unrolled_list(unrolled_list&& other, const allocator_type& allocator)
        : node_allocator_(allocator) {
        if (node_allocator_ == other.alloc) {
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head = nullptr;
            other.tail = nullptr;
            other.total_count = 0;
        } else {
            try {
                for (const auto& value : other) {
                    push_back(value);
                }
                other.clear();
            } catch (...) {
                clear();
                throw;
            }
        }
    }

    ~unrolled_list() {
        clear();
    }

    class iterator {

    public:

        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        iterator(Node* node = nullptr, size_type index = 0, unrolled_list* list = nullptr)
            : node_(node), index_(index), list_(list) {}

        iterator(const iterator& other) : node_(other.node_), index_(other.index_), list_(other.list_) {}

        reference operator*() const {
            if (!node_){
                throw;
            }
            return node_->data_[index_];
        }

        pointer operator->() const {
            if (!node_){
                throw;
            }
            return &node_->data_[index_];
        }

        iterator& operator++() {
            if (node_ && index_ + 1 < node_->data_size_) {
                ++index_;
            }
            else {
                node_ = node_ ? node_->next_ : nullptr;
                index_ = 0;
            }

            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator operator+(int n){
            iterator temp = *this;
            for (int i = 0; i < n; ++i) {
                ++temp;
            }

            return temp;
        }

        iterator& operator+=(int n){
            for (int i = 0; i < n; ++i) {
                ++(*this);
            }
            return *this;
        }

        iterator& operator--() {
            if (node_) {
                if (index_ > 0) {
                    --index_;
                }
                else {
                    node_ = node_->prev_;
                    if (node_) {
                        index_ = node_->data_size_ - 1;
                    }
                }
            }

            else {
                if (list_->get_tail()) {
                    node_ = list_->get_tail();
                    index_ = list_->get_tail()->data_size_ - 1;
                }
            }

            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }

        iterator operator-(int n){
            iterator temp = *this;
            for (int i = 0; i < n; ++i) {
                --temp;
            }

            return temp;
        }

        iterator& operator-=(int n){
            for (int i = 0; i < n; ++i) {
                --(*this);
            }
            return *this;
        }

        bool operator==(const iterator& other) const {
            return node_ == other.node_ && index_ == other.index_;
        }
    
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
        
        Node* getNode() const {
            return node_;
        }

        size_type getIndex() const {
            return index_;
        }

    private:

        Node* node_;
        size_type index_;
        const unrolled_list* list_;

    };

    class const_iterator {
    
    public:

        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        const_iterator(Node* node = nullptr, size_type index = 0, unrolled_list* list = nullptr)
            : node_(node), index_(index), list_(list) {}
        
        const_iterator(const const_iterator& other)
            : node_(other.node_), index_(other.index_), list_(other.list_) {}

        const_reference operator*() const {
            return node_->data_[index_];
        }

        pointer operator->() const {
            return &node_->data_[index_];
        }

        const_iterator& operator++() {
            if (node_ && index_ + 1 < node_->data_size_) {
                ++index_;
            } else {
                node_ = node_ ? node_->next_ : nullptr;
                index_ = 0;
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        const_iterator operator+(int n){
            iterator temp = *this;
            for (int i = 0; i < n; ++i) {
                ++temp;
            }

            return temp;
        }

        const_iterator& operator+=(int n){
            for (int i = 0; i < n; ++i) {
                ++(*this);
            }
            return *this;
        }

        const_iterator& operator--() {
            if (node_) {
                if (index_ > 0) {
                    --index_;
                }
                else {
                    node_ = node_->prev_;
                    if (node_) {
                        index_ = node_->data_size_ - 1;
                    }
                }
            }
    
            else {
                if (list_->get_tail()) {
                    node_ = list_->get_tail();
                    index_ = list_->get_tail()->data_size_ - 1;
                }
            }
    
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }

        const_iterator operator-(int n){
            iterator temp = *this;
            for (int i = 0; i < n; ++i) {
                --temp;
            }

            return temp;
        }

        const_iterator& operator-=(int n){
            for (int i = 0; i < n; ++i) {
                --(*this);
            }
            return *this;
        }

        bool operator==(const const_iterator& other) const {
            return node_ == other.node_ && index_ == other.index_;
        }
    
        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }

        Node* getNode() const {
            return node_;
        }

        size_type getIndex() const {
            return index_;
        }

    private:

        Node* node_;
        size_type index_;
        const unrolled_list* list_;

    };

    iterator begin() {
        return iterator(head_, 0);
    }

    iterator end() {
        return iterator(nullptr, 0);
    }

    const_iterator begin() const {
        return const_iterator(head_, 0);
    }

    const_iterator end() const {
        return const_iterator(nullptr, 0);
    }

    const_iterator cbegin() const {
        return const_iterator(head_, 0);
    }

    const_iterator cend() const {
        return const_iterator(nullptr, 0);
    }

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return rbegin();
    }

    const_reverse_iterator crend() const {
        return rend();
    }

    reference front() {
        if (empty()) {
            throw std::out_of_range("list is empty");
        }
        return head_->data_[0];
    }

    const_reference front() const {
        if (empty()) {
            throw std::out_of_range("list is empty");
        }
        return head_->data_[0];
    }

    reference back() {
        if (empty()) {
            throw std::out_of_range("list is empty");
        }
        return tail_->data_[tail_->data_size_ - 1];
    }

    const_reference back() const {
        if (empty()) {
            throw std::out_of_range("list is empty");
        }
        return tail_->data_[tail_->data_size_ - 1];
    }

    iterator insert(iterator elem, const_reference value) {
        Node* new_node = nullptr;

        if (elem == end()) {
            if (!tail_ || tail_->data_size_ == NodeMaxSize) {
                new_node = allocate_node();

                try {
                    new (&new_node->data_[0]) T(value);

                    if (!tail_) {
                        head_ = new_node;
                        tail_ = new_node;
                    } 
        
                    else {
                        tail_->next_ = new_node;
                        new_node->prev_ = tail_;
                        tail_ = new_node;
                    }

                    ++new_node->data_size_;
                    ++size_;
                    return iterator(tail_, 0);
                } catch (...) {
                    if (new_node != nullptr) { 
                        deallocate_node(new_node);
                    }
                    throw;
                }
            }
    
            else {
                try {
                    new (&tail_->data_[tail_->data_size_]) T(value);
                    ++tail_->data_size_;
                    ++size_;
                    return iterator(tail_, tail_->data_size_ - 1);
                } catch (...) {
                    if (new_node != nullptr) { 
                        deallocate_node(new_node);
                    }
                    throw;
                }
            }
        }

        pointer temp_data1 = nullptr;
        pointer temp_data2 = nullptr;
        pointer old_data1 = nullptr;
        pointer old_data2 = nullptr;

        if (elem.getNode()->data_size_ < NodeMaxSize) {
            try {
                temp_data1 = static_cast<pointer>(operator new(NodeMaxSize * sizeof(T)));
                pointer old_data = elem.getNode()->data_;

                for (size_type i = elem.getNode()->data_size_; i > elem.getIndex(); --i) {
                    new (&temp_data1[i]) T(elem.getNode()->data_[i - 1]);
                }

                for (size_type i = 0; i < elem.getIndex(); ++i) {
                    new (&temp_data1[i]) T(elem.getNode()->data_[i]);
                }

                new (&temp_data1[elem.getIndex()]) T(value);

                for (size_type i = 0; i < elem.getNode()->data_size_; ++i) {
                    (&elem.getNode()->data_[i])->~T();
                }

                elem.getNode()->data_ = temp_data1;
                ++elem.getNode()->data_size_;
                ++size_;
                
                operator delete(old_data);
                temp_data1 = nullptr;

                return elem;
            } catch (...) {
                if (temp_data1 != nullptr){
                    for (size_type i = 0; i < elem.getNode()->data_size_; ++i) {
                        (&temp_data1[i])->~T();
                    }

                    operator delete(temp_data1);
                    temp_data1 = nullptr;
                }

                throw;
            }
        }

        else {
            try {
                new_node = allocate_node();
                old_data1 = elem.getNode()->data_;
                old_data2 = new_node->data_;
                temp_data1 = static_cast<pointer>(operator new(NodeMaxSize * sizeof(T)));
                temp_data2 = static_cast<pointer>(operator new(NodeMaxSize * sizeof(T)));
                
                if (elem.getIndex() <= NodeMaxSize / 2) {
                    size_type half_size = (NodeMaxSize % 2 == 0) ? NodeMaxSize / 2 - 1 : NodeMaxSize / 2; 

                    for (size_type i = 0; i < elem.getIndex(); ++i) {
                        new (&temp_data1[i]) T(elem.getNode()->data_[i]);
                    }

                    for (size_type i = elem.getIndex(); i < NodeMaxSize / 2; ++i) {
                        new (&temp_data1[i+1]) T(elem.getNode()->data_[i]);
                    }

                    new (&temp_data1[elem.getIndex()]) T(value);

                    for (size_type i = NodeMaxSize / 2; i < NodeMaxSize; ++i) {
                        new (&temp_data2[i - NodeMaxSize / 2]) T(elem.getNode()->data_[i]);
                    }

                    for (size_type i = 0; i < NodeMaxSize; ++i) {
                        (&elem.getNode()->data_[i])->~T();
                    }

                    new_node->next_ = elem.getNode()->next_;
                    if (new_node->next_ != nullptr) {
                        new_node->next_->prev_ = new_node;
                    }
                    elem.getNode()->next_ = new_node;
                    new_node->prev_ = elem.getNode();

                    elem.getNode()->data_ = temp_data1;
                    new_node->data_ = temp_data2;


                    elem.getNode()->data_size_ = NodeMaxSize - half_size;
                    new_node->data_size_ = half_size + 1 ;

                    operator delete(old_data1);
                    operator delete(old_data2);

                    temp_data1 = nullptr;
                    temp_data2 = nullptr;

                    ++size_;

                    if (elem.getNode() == tail_) {
                        tail_ = new_node;
                    }

                    return iterator(elem.getNode(), elem.getIndex(), this);
                }

                else {
                    size_type half_size = (NodeMaxSize % 2 == 0) ? NodeMaxSize / 2 - 1 : NodeMaxSize / 2;

                    for (size_type i = 0; i <= half_size; ++i) {
                        new (&temp_data1[i]) T(elem.getNode()->data_[i]);
                    }

                    for (size_type i = half_size + 1; i < elem.getIndex(); ++i) {
                        new (&temp_data2[i - half_size - 1]) T(elem.getNode()->data_[i]);
                    }

                    for (size_type i = elem.getIndex(); i < NodeMaxSize; ++i) {
                        new (&temp_data2[i - half_size]) T(elem.getNode()->data_[i]);
                    }

                    new (&temp_data2[elem.getIndex() - half_size - 1]) T(value);

                    for (size_type i = 0; i < NodeMaxSize; ++i) {
                        (&elem.getNode()->data_[i])->~T();
                    }

                    new_node->next_ = elem.getNode()->next_;
                    if (new_node->next_ != nullptr) {
                        new_node->next_->prev_ = new_node;
                    }
                    elem.getNode()->next_ = new_node;
                    new_node->prev_ = elem.getNode();

                    elem.getNode()->data_ = temp_data1;
                    new_node->data_ = temp_data2;

                    elem.getNode()->data_size_ = half_size + 1;
                    new_node->data_size_ = NodeMaxSize - half_size;

                    operator delete(old_data1);
                    operator delete(old_data2);

                    temp_data1 = nullptr;
                    temp_data2 = nullptr;

                    ++size_;

                    if (elem.getNode() == tail_) {
                        tail_ = new_node;
                    }

                    return iterator(elem.getNode(), elem.getIndex(), this);
                }

            } catch (...) {
                if (new_node != nullptr) {
                    for (size_t i = 0; i < new_node->data_size_; ++i) {
                        (&new_node->data_[i])->~T();
                    }
                    deallocate_node(new_node);
                    new_node = nullptr;
                }

                if (temp_data1 != nullptr){
                    for (size_type i = 0; i < elem.getNode()->data_size_; ++i) {
                        (&temp_data1[i])->~T();
                    }

                    operator delete(temp_data1);
                    temp_data1 = nullptr;
                }

                if (temp_data2 != nullptr){
                    for (size_type i = 0; i < elem.getNode()->data_size_; ++i) {
                        (&temp_data2[i])->~T();
                    }

                    operator delete(temp_data2);
                    temp_data2 = nullptr;
                }

                old_data1 = nullptr;
                old_data2 = nullptr;

                throw;
            }
        }    
    }

    iterator insert(const_iterator elem, const_reference value) {
        return insert(iterator(elem.getNode(), elem.getIndex(), this), value);
    }

    iterator insert(iterator elem, size_type n, const value_type& value) {
        if (n == 0) {
            return elem;
        }

        iterator result(elem);
        for (size_type i = 0; i < n; ++i) {
            result = insert(result, value);
        }

        return result;
    }

    iterator insert(const_iterator elem, size_type n, const_reference value) {
        if (n == 0) {
            return iterator(elem.getNode(), elem.getIndex(), this);
        }

        iterator result(elem.getNode(), elem.getIndex(), this);
        for (size_type i = 0; i < n; ++i) {
            result = insert(result, value);
        }

        return result;
    }

    template<typename InputIterator>
    void assign(InputIterator first, InputIterator last) {
        clear();

        try {
            while (first != last) {
                push_back(*first);
                ++first;
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    void assign(std::initializer_list<value_type> obj) {
        assign(obj.begin(), obj.end());
    }

    iterator erase(iterator elem) noexcept {
        if (elem == end()) {
            return end();
        }

        Node* current_node = elem.getNode();
        size_type index_to_erase = elem.getIndex();

        (&current_node->data_[index_to_erase])->~T();
        --current_node->data_size_;
        --size_;

        if (index_to_erase < current_node->data_size_) { 
            for (size_type i = index_to_erase; i < current_node->data_size_; ++i) {
                (&current_node->data_[i])->~T();
                new (&current_node->data_[i]) T(current_node->data_[i + 1]);
            }
            return iterator(current_node, index_to_erase);
        }

        if (current_node->data_size_ == 0) {
            Node* next_node = current_node->next_;

            if (current_node != head_ && current_node != tail_) {
                current_node->prev_->next_ = current_node->next_;
                current_node->next_->prev_ = current_node->prev_;
            }

            if (current_node == head_) {
                head_ = current_node->next_;
                if (head_) { 
                    head_->prev_ = nullptr;
                }
            } 

            if (current_node == tail_) {
                tail_ = current_node->prev_;
                if (tail_) {
                    tail_->next_ = nullptr;
                }
            } 

            deallocate_node(current_node);

            if (next_node) { 
                return iterator(next_node, 0); 
            }

            return end();
        }

        return end();
    }

    iterator erase(const_iterator elem) noexcept {
        return erase(iterator(elem.getNode(), elem.getIndex(), this));
    }

    iterator erase(const_iterator first, const_iterator last) noexcept {
        Node* node = first.getNode();
        size_t index = first.getIndex();

        while (first != last) {
            first = erase(first);
        }

        return iterator(node, index, this);
    }

    void push_back(const_reference value) {
        insert(end(), value);
    }

    void pop_back() noexcept {
        if (tail_){
            erase(iterator(tail_, tail_->data_size_ - 1));
        }
    }

    void push_front(const_reference value){
        insert(begin(), value);
    }

    void pop_front() noexcept {
        erase(begin());
    }

    void clear() noexcept {
        Node* current = head_;

        while (current != nullptr) {
            Node* next = current->next_;
            deallocate_node(current);
            current = next;
        }

        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    size_type size() const {
        return size_;
    }
    
    size_type max_size() const noexcept {
        return std::allocator_traits<allocator_type>::max_size(node_allocator_);
    }

    bool empty() const {
        return size_==0;
    }

    unrolled_list& operator=(const unrolled_list& other) {
        if (this != &other) {
            clear();

            try {
                for (const auto& value : other) {
                    push_back(value);
                }
            } catch (...) {
                clear();
                throw;
            }
        }
        return *this;
    }

    bool operator==(const unrolled_list& other) const {
        if (size_ != other.size_) {
            return false;
        }

        auto iter1 = begin();
        auto iter2 = other.begin();

        while (iter1 != end() && iter2 != other.end()) {
            if (*iter1 != *iter2) {
                return false;
            }

            ++iter1;
            ++iter2;
        }

        return true;
    }

    bool operator!=(const unrolled_list& other) const {
        return !(*this == other);
    }

    allocator_type get_allocator() const {
        return node_allocator_;
    }

    void print_list_by_nodes() const {
        for (Node* current = head_; current != nullptr; current = current->next_) {
            std::cout << "[";
            for (size_t i = 0; i < current->data_size_; ++i) {
                std::cout << current->data_[i] << " ";
            }
            std::cout << "] - " << current->data_size_ << ", ";
        }
        std::cout << std::endl;
    }

    void print_list() const {
        for (Node* current = head_; current != nullptr; current = current->next_) {
            for (size_t i = 0; i < current->data_size_; ++i) {
                std::cout << current->data_[i] << " ";
            }
        }
        std::cout << std::endl;
    }

    Node* get_tail() const {
        return tail_;
    }

    Node* get_head() const {
        return head_;
    }

};