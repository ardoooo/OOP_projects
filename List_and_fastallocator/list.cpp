#pragma once
#include <vector>
#include <memory>
#include <type_traits>


template <size_t chunk_size>
class FixedAllocator {
private:
    struct Pool {
        int8_t* pool;
        int8_t* free_place;
        static const size_t size_pool = 200000000;
        Pool() {
            pool = new int8_t[size_pool];
            free_place = pool;
        }
        ~Pool() {
            delete[] pool;
        }
        void* inc() {
            free_place += chunk_size;
            return reinterpret_cast<void*>(free_place - chunk_size);
        }
    };
    std::shared_ptr<Pool> pool;
public:
    FixedAllocator() {
        pool = std::make_shared<Pool>();
    };
    ~FixedAllocator() {
        pool = nullptr;
    }
    

    void* allocate() {
        return pool->inc();
    }
    void deallocate(void*) {
    };
};

template<size_t T, size_t U>
bool operator==(const FixedAllocator<T>&, const FixedAllocator<U>&) {
    return std::is_same_v<T, U>;
}


template <typename T>
class FastAllocator {
private:
    std::shared_ptr<FixedAllocator<sizeof(T)>> fixed_alloc;
public:
    FastAllocator() {
        fixed_alloc = std::make_shared<FixedAllocator<sizeof(T)>>();
    };
    ~FastAllocator() {
        fixed_alloc = nullptr;
    };
    FastAllocator(const FastAllocator<T>& other) {
        fixed_alloc = other.fixed_alloc;
    }
    T* allocate(size_t n) {
        if (n * sizeof(T) <= 32) {
            return reinterpret_cast<T*>(fixed_alloc->allocate());
        }
        else {
            return reinterpret_cast<T*>(new int8_t[n * sizeof(T)]);
        }

    }
    void deallocate(T* ptr, size_t n) {
        if (n * sizeof(T) <= 32) {
            fixed_alloc->deallocate(reinterpret_cast<void*>(ptr));
        }
        else {
            delete[] reinterpret_cast<int8_t*>(ptr);
        }

    }

    using value_type = T;

    template<class U>
    struct rebind {
        typedef FastAllocator<U> other;
    };
    template<class U>
    explicit operator FastAllocator<U>() const { return FastAllocator<U>(); }
};

template<class T, class U>
bool operator==(const FastAllocator<T>&, const FastAllocator<U>&) {
    return std::is_same_v<T, U>;
}


template<class T, class U>
bool operator!=(const FastAllocator<T>& a, const FastAllocator<U>& b) {
    return !(a == b);
}



template<typename T, typename Allocator = std::allocator<T>>
class List {
private:
    struct Node {
        T value;
        Node* next;
        Node* prev;
        Node() = default;
        Node(const T& value) : value(value), next(nullptr), prev(nullptr) {};
        ~Node() = default;
        bool operator==(const Node& other) {
            if (next != other.next) return false;
            if (prev != other.prev) return false;
            if (value != other.value) return false;
            return true;
        }
    };
public:

    explicit List(const Allocator& alloc = Allocator()) : _size(0), alloc(alloc) {
        Node* head_node = alloc_traits_node::allocate(alloc_node, 1);
        Node* tail_node = alloc_traits_node::allocate(alloc_node, 1);
        head = head_node;
        tail = tail_node;
        head->prev = nullptr;
        head->next = tail;
        tail->prev = head;
        tail->next = nullptr;
    }

    List(size_t count, const Allocator& alloc = Allocator()) : _size(count), alloc(alloc) {
        Node* prev_node = nullptr;
        Node* start_node = alloc_traits_node::allocate(alloc_node, 1);
        start_node->prev = nullptr;
        prev_node = start_node;
        for (size_t i = 0; i < count; ++i) {
            Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
            alloc_traits_node::construct(alloc_node, new_node);
            prev_node->next = new_node;
            new_node->prev = prev_node;
            prev_node = new_node;
        }
        Node* end_node = alloc_traits_node::allocate(alloc_node, 1);
        prev_node->next = end_node;
        end_node->prev = prev_node;
        end_node->next = nullptr;

        head = start_node;
        tail = end_node;
    }
    List(size_t count, const T& value, const Allocator& alloc = Allocator()) : _size(count), alloc(alloc) {
        Node* prev_node = nullptr;
        Node* start_node = alloc_traits_node::allocate(alloc_node, 1);
        start_node->prev = nullptr;
        prev_node = start_node;
        for (size_t i = 0; i < count; ++i) {
            Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
            alloc_traits_node::construct(alloc_node, new_node, value);
            prev_node->next = new_node;
            new_node->prev = prev_node;
            prev_node = new_node;
        }
        Node* end_node = alloc_traits_node::allocate(alloc_node, 1);
        prev_node->next = end_node;
        end_node->prev = prev_node;
        end_node->next = nullptr;

        head = start_node;
        tail = end_node;
    }
    List(const List& other) : _size(other._size) {
        alloc = alloc_traits::select_on_container_copy_construction(other.alloc);
        Node* prev_node = nullptr;
        Node* start_node = alloc_traits_node::allocate(alloc_node, 1);
        start_node->prev = nullptr;
        prev_node = start_node;

        Node* copy_node = other.head->next;

        for (size_t i = 0; i < _size; ++i) {
            Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
            alloc_traits_node::construct(alloc_node, new_node, copy_node->value);
            prev_node->next = new_node;
            new_node->prev = prev_node;
            prev_node = new_node;

            copy_node = copy_node->next;
        }
        Node* end_node = alloc_traits_node::allocate(alloc_node, 1);
        prev_node->next = end_node;
        end_node->prev = prev_node;
        end_node->next = nullptr;

        head = start_node;
        tail = end_node;
    }

    List& operator=(const List& other) {
        Node* next_node = head;
        while (next_node != nullptr) {
            Node* tmp = next_node;
            next_node = next_node->next;
            if (tmp->next != nullptr && tmp->prev != nullptr) {
                alloc_traits_node::destroy(alloc_node, tmp);
            }
            alloc_traits_node::deallocate(alloc_node, tmp, 1);
        }

        if (alloc_traits::propagate_on_container_copy_assignment::value && alloc != other.alloc) {
            alloc = other.alloc;
        }
        _size = other._size;

        Node* prev_node = nullptr;
        Node* start_node = alloc_traits_node::allocate(alloc_node, 1);
        start_node->prev = nullptr;
        prev_node = start_node;

        Node* copy_node = other.head->next;

        for (size_t i = 0; i < _size; ++i) {
            Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
            alloc_traits_node::construct(alloc_node, new_node, copy_node->value);
            prev_node->next = new_node;
            new_node->prev = prev_node;
            prev_node = new_node;

            copy_node = copy_node->next;
        }
        Node* end_node = alloc_traits_node::allocate(alloc_node, 1);
        prev_node->next = end_node;
        end_node->prev = prev_node;
        end_node->next = nullptr;

        head = start_node;
        tail = end_node;
        return *this;
    }
    ~List() {
        Node* next_node = head;
        while (next_node != nullptr) {
            Node* tmp = next_node;
            next_node = next_node->next;
            if (tmp->next != nullptr && tmp->prev != nullptr) {
                alloc_traits_node::destroy(alloc_node, tmp);
            }
            alloc_traits_node::deallocate(alloc_node, tmp, 1);
        }
    }

    Allocator get_allocator() const {
        return alloc;
    }

    size_t size() const {
        return _size;
    }

    void push_back(const T& value) {
        ++_size;
        Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
        alloc_traits_node::construct(alloc_node, new_node, value);
        new_node->next = tail;
        new_node->prev = tail->prev;
        new_node->prev->next = new_node;
        new_node->next->prev = new_node;
    }

    void push_front(const T& value) {
        ++_size;
        Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
        alloc_traits_node::construct(alloc_node, new_node, value);
        new_node->prev = head;
        new_node->next = head->next;
        new_node->prev->next = new_node;
        new_node->next->prev = new_node;
    }

    void pop_back() {
        --_size;
        Node* tmp = tail->prev;
        tail->prev = tmp->prev;
        tmp->prev->next = tail;
        alloc_traits_node::destroy(alloc_node, tmp);
        alloc_traits_node::deallocate(alloc_node, tmp, 1);
    }
    void pop_front() {
        --_size;
        Node* tmp = head->next;
        head->next = tmp->next;
        tmp->next->prev = head;
        alloc_traits_node::destroy(alloc_node, tmp);
        alloc_traits_node::deallocate(alloc_node, tmp, 1);
    }

    //Iterators
    template <bool is_const>
    struct common_iterator {

        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::conditional_t<is_const, const T, T>;
        using pointer = std::conditional_t<is_const, const T*, T*>;
        using reference = std::conditional_t<is_const, const T&, T&>;
        friend class List<T, Allocator>;

    private:
        Node* ptr;
    public:
        common_iterator(Node* ptr) : ptr(ptr) {}
        common_iterator(const common_iterator& other) : ptr(other.ptr) {}
        common_iterator& operator=(const common_iterator& other) {
            ptr = other.ptr;
            return *this;
        };
        std::conditional_t<is_const, const T&, T&> operator*() {
            return ptr->value;
        }

        std::conditional_t<is_const, const T*, T*> operator->() {
            return &(ptr->value);
        }

        common_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }
        common_iterator operator++(int) {
            common_iterator tmp(ptr);
            ptr = ptr->next;
            return tmp;
        }
        common_iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }

        common_iterator operator--(int) {
            common_iterator tmp(ptr);
            ptr = ptr->prev;
            return tmp;
        }
        bool operator==(const common_iterator& other) {
            return (*ptr == *other.ptr);
        }
        bool operator!=(const common_iterator& other) {
            return !(*ptr == *other.ptr);
        }

        template<bool IsConst2>
        explicit operator common_iterator<IsConst2>() {
            common_iterator<IsConst2> copy = common_iterator<IsConst2>(ptr);
            return copy;
        }

    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(head->next);
    }
    iterator end() {
        return iterator(tail);
    }
    const_iterator cbegin() const {
        return const_iterator(head->next);
    }
    const_iterator cend() const {
        return const_iterator(tail);
    }
    const_iterator begin() const {
        return cbegin();
    }
    const_iterator end() const {
        return cend();
    }
    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }
    reverse_iterator rend() {
        return reverse_iterator(begin());
    }
    reverse_iterator rbegin() const {
        auto it = const_reverse_iterator(tail);
        return it;
    }
    reverse_iterator rend() const {
        auto it = const_reverse_iterator(head->next);
        return it;
    }
    const_reverse_iterator crbegin() const {
        auto it = const_reverse_iterator(tail);
        return it;
    }
    const_reverse_iterator crend() const {
        auto it = const_reverse_iterator(head->next);
        return it;
    }

    template <typename _iterator>
    void insert(_iterator it, const T& value) {
        ++_size;
        Node* new_node = alloc_traits_node::allocate(alloc_node, 1);
        alloc_traits_node::construct(alloc_node, new_node, value);
        new_node->next = it.ptr;
        new_node->prev = it.ptr->prev;
        new_node->prev->next = new_node;
        new_node->next->prev = new_node;
    }

    template <typename _iterator>
    void erase(_iterator it) {
        --_size;
        Node* tmp = it.ptr;
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        alloc_traits_node::destroy(alloc_node, tmp);
        alloc_traits_node::deallocate(alloc_node, tmp, 1);
    }
private:
    Node* head;
    Node* tail;
    size_t _size;
    Allocator alloc;
    using alloc_traits = std::allocator_traits<Allocator>;
    typename alloc_traits::template rebind_alloc<Node> alloc_node;
    using alloc_traits_node = std::allocator_traits<typename alloc_traits::template rebind_alloc<Node>>;
};
