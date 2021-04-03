#pragma once
#include <iostream>
#include <vector>
#include <iterator>
#include <cmath>

using std::vector;

template <typename T>
class Deque {
private:

    size_t _size, count_reserve_chunks;
    vector<T*> chunks;
    std::pair<size_t, size_t> begin_position, end_position;

public:
    static const size_t CHUNK_SIZE = 1024;
    size_t zero_of_coordinates;
    Deque();
    Deque(const Deque& other);
    Deque(int t);
    Deque(int t, const T&);

    ~Deque();

    Deque& operator=(const Deque& other);

    size_t size() const;
    size_t capacity() const;

    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    T& at(size_t index);
    const T& at(size_t index) const;

    //Iterators
    template <bool is_const>
    struct common_iterator {

        using iterator_category = std::random_access_iterator_tag;
        using difference_type = long long;
        using value_type = std::conditional_t<is_const, const T, T>;
        using pointer = std::conditional_t<is_const, const T*, T*>;
        using reference = std::conditional_t<is_const, const T&, T&>;
        friend class Deque;

    private:
        T* const* pointer_home_chunk;
        std::conditional_t<is_const, const T*, T*> ptr;
        std::pair<size_t, size_t> position, begin_position, end_position;
    public:
        common_iterator(std::pair<size_t, size_t> pos, T* const* pointer_home_chunk,
            std::pair<size_t, size_t> begin_position, std::pair<size_t, size_t> end_position) : pointer_home_chunk(pointer_home_chunk),
            position(pos), begin_position(begin_position), end_position(end_position) {
            if (position == end_position) {
                ptr = nullptr;
            }
            else {
                ptr = *pointer_home_chunk + position.second;
            }
        }

        std::conditional_t<is_const, const T&, T&> operator*() {
            return *ptr;
        }

        std::conditional_t<is_const, const T*, T*> operator->() {
            return ptr;
        }

        common_iterator& operator++() {
            if (position.second == Deque::CHUNK_SIZE - 1) {
                position.second = 0;
                ++position.first;
                ++pointer_home_chunk;
            }
            else {
                ++position.second;
            }
            if (position == end_position) {
                ptr = nullptr;
            }
            else {
                ptr = *pointer_home_chunk + position.second;
            }
            return *this;
        }
        common_iterator& operator--() {
            if (position.second == 0) {
                position.second = Deque::CHUNK_SIZE - 1;
                --position.first;
                --pointer_home_chunk;
            }
            else {
                --position.second;
            }
            if (position.first == begin_position.first - 1) {
                ptr = nullptr;
            }
            else {
                ptr = *pointer_home_chunk + position.second;
            }
            return *this;
        }

        long long operator-(const common_iterator& other) const {
            return (static_cast<long long>(position.first) - other.position.first) * Deque::CHUNK_SIZE
                + position.second - other.position.second;
        }

        friend common_iterator operator+(const common_iterator& it, long long n) {
            long long new_global_position = static_cast<long long>(it.position.first) * Deque::CHUNK_SIZE + it.position.second + n;
            return common_iterator({ new_global_position / Deque::CHUNK_SIZE, new_global_position % Deque::CHUNK_SIZE }, 
                it.pointer_home_chunk + new_global_position / Deque::CHUNK_SIZE - it.position.first,
                it.begin_position, it.end_position);
        }
        friend common_iterator operator-(const common_iterator& it, long long n) {
            long long new_global_position = static_cast<long long>(it.position.first) * Deque::CHUNK_SIZE + it.position.second - n;
            return common_iterator({ new_global_position / Deque::CHUNK_SIZE, new_global_position % Deque::CHUNK_SIZE }, 
                it.pointer_home_chunk + new_global_position / Deque::CHUNK_SIZE - it.position.first, 
                it.begin_position, it.end_position);
        }
        bool operator==(const common_iterator& other) {
            return (*this - other) == 0;
        }
        bool operator!=(const common_iterator& other) {
            return (*this - other) != 0;
        }
        bool operator<(const common_iterator& other) {
            return (*this - other) < 0;
        }
        bool operator<=(const common_iterator& other) {
            return (*this - other) <= 0;
        }
        bool operator>(const common_iterator& other) {
            return (*this - other) > 0;
        }
        bool operator>=(const common_iterator& other) {
            return (*this - other) >= 0;
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(begin_position, &chunks[begin_position.first], begin_position, end_position);
    }
    iterator end() {
        return iterator(end_position, &chunks[end_position.first], begin_position, end_position);
    }
    const_iterator cbegin() const {
        return const_iterator(begin_position, &chunks[begin_position.first], begin_position, end_position);
    }
    const_iterator cend() const {
        return const_iterator(end_position, &chunks[end_position.first], begin_position, end_position);
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
    const_reverse_iterator crbegin() const {
        return cend();
    }
    const_reverse_iterator crend() const {
        return cbegin();
    }

    //modification functions
    void update_of_reserved(double value);
    void expand();
    void shrink();
    void push_back(const T& value);
    void pop_back();
    void push_front(const T& value);
    void pop_front();
    void insert(iterator, const T& value);
    void erase(iterator);
};

template <typename T>
Deque<T>::Deque() : _size(0), count_reserve_chunks(2), zero_of_coordinates(0) {
    chunks.resize(2, nullptr);
    chunks[0] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    chunks[1] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    begin_position = { 0, CHUNK_SIZE - 1 };
    end_position = { 0, CHUNK_SIZE - 1 };
}

template <typename T>
Deque<T>::Deque(int t) : _size(t), zero_of_coordinates(0) {
    count_reserve_chunks = ceil(static_cast<float>(t) / static_cast<float>(CHUNK_SIZE)) + 1;
    chunks.resize(count_reserve_chunks, nullptr);
    for (size_t i = 0; i < count_reserve_chunks; ++i) {
        chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
    /*size_t mod = ((t - 1) % CHUNK_SIZE + 1) % CHUNK_SIZE;
    begin_position = { 0, CHUNK_SIZE - 1 };
    end_position = { count_reserve_chunks - 1 + (mod == 0), mod };*/
    int mod = t % CHUNK_SIZE;
    begin_position = { 0, CHUNK_SIZE - 1 };
    end_position = { count_reserve_chunks - 1 - (mod == 0), ((mod + CHUNK_SIZE) - 1) % CHUNK_SIZE };
}

template <typename T>
Deque<T>::Deque(int t, const T& value) : _size(t), zero_of_coordinates(0) {
    count_reserve_chunks = ceil(static_cast<float>(t) / static_cast<float>(CHUNK_SIZE)) + 1;
    chunks.resize(count_reserve_chunks, nullptr);
    /*size_t mod = (t - 1) % CHUNK_SIZE;

    begin_position = { 0, CHUNK_SIZE - 1 };
    end_position = { count_reserve_chunks - 1 + (mod == 0), mod};
    */
    int mod = t % CHUNK_SIZE;
    begin_position = { 0, CHUNK_SIZE - 1 };
    end_position = { count_reserve_chunks - 1 - (mod == 0), ((mod + CHUNK_SIZE) - 1) % CHUNK_SIZE };

    size_t i = 1; size_t j = 0;
    try {
        for (; i < count_reserve_chunks - 1; ++i) {
            chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
            for (j = 0; j < CHUNK_SIZE; ++j) {
                new(chunks[i] + j) T(value);
            }
        }
    } catch (...) {

        for (size_t k = 0; k < i; ++k) {
            for (size_t l = 0; l < CHUNK_SIZE; ++l) {
                (chunks[k] + l)->~T();
            }
            delete[] reinterpret_cast<int8_t*>(chunks[k]);
        }

        for (size_t l = 0; l < j; ++l) {
            (chunks[i] + l)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[i]);
        throw;
    }


    chunks[0] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    try {
        for (j = begin_position.second; j < CHUNK_SIZE; ++j) {
            new(chunks[0] + j) T(value);
        }
    } catch (...) {
        for (size_t l = begin_position.second; l < j; ++l) {
            (chunks[0] + l)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[0]);
        throw;
    }


    chunks[count_reserve_chunks - 1] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    try {
        if (end_position.second != 0) {
            for (j = 0; j < end_position.second; ++j) {
                new(chunks[count_reserve_chunks - 1] + j) T(value);
            }
        } else {
            for (j = 0; j < CHUNK_SIZE; ++j) {
                new(chunks[count_reserve_chunks - 1] + j) T(value);
            }
        }
    } catch (...) {
        for (size_t k = 0; k < j; ++k) {
            (chunks[count_reserve_chunks - 1] + k)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[count_reserve_chunks - 1]);
        throw;
    }
}

template <typename T>
Deque<T>::Deque(const Deque& other) {
    _size = other._size;
    zero_of_coordinates = other.zero_of_coordinates;
    count_reserve_chunks = other.count_reserve_chunks;
    begin_position = other.begin_position;
    end_position = other.end_position;
    chunks.resize(other.count_reserve_chunks, nullptr);
   
    size_t i; size_t j;
    try {
        for (i = begin_position.first + 1; i < end_position.first; ++i) {
            chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
            for (j = 0; j < CHUNK_SIZE; ++j) {
                new(chunks[i] + j) T(other.chunks[i][j]);
            }
        }
    } catch (...) {
        for (size_t k = 0; k < i; ++k) {
            for (size_t l = 0; l < CHUNK_SIZE; ++l) {
                (chunks[k] + l)->~T();
            }
            delete[] reinterpret_cast<int8_t*>(chunks[k]);
        }

        for (size_t l = 0; l < j; ++l) {
            (chunks[i] + l)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[i]);
        throw;
    }

    if (end_position.second != 0) {
        chunks[end_position.first] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
        try {
            for (j = 0; j < end_position.second; ++j) {
                new(chunks[end_position.first] + j) T(other.chunks[i][j]);
            }
        }
        catch (...) {
            for (size_t k = 0; k < j; ++k) {
                (chunks[end_position.first] + k)->~T();
            }
            delete[] reinterpret_cast<int8_t*>(chunks[end_position.first]);
            throw;
        }
    } 

    chunks[begin_position.first] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    try {
        if (begin_position.first != end_position.first) {
            for (j = begin_position.second; j < CHUNK_SIZE; ++j) {
                new(chunks[begin_position.first] + j) T(other.chunks[begin_position.first][j]);
            }
        } else {
            for (j = begin_position.second; j < end_position.second; ++j) {
                new(chunks[begin_position.first] + j) T(other.chunks[begin_position.first][j]);
            }
        }
    } catch(...) { 
        for (size_t l = begin_position.second; l < j; ++l) {
            (chunks[begin_position.first] + l)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[0]);
        throw;
    }


    for (i = 0; i < begin_position.first; ++i) {
        chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
    for (i = end_position.first + 1; i < chunks.size(); ++i) {
        chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
}

template <typename T>
Deque<T>::~Deque() {
    if (begin_position.first != end_position.first) {
        for (size_t i = begin_position.second; i < CHUNK_SIZE; ++i) {
            (chunks[begin_position.first] + i)->~T();
        }
    } else {
        for (size_t i = begin_position.second; i < end_position.second; ++i) {
            (chunks[begin_position.first] + i)->~T();
        }
    }

    for (size_t i = begin_position.first + 1; i < end_position.first; ++i) {
        for (size_t j = 0; j < CHUNK_SIZE; ++j) {
            (chunks[i] + j)->~T();
        }
    }
    for (size_t i = 0; i < end_position.second; ++i) {
        (chunks[end_position.first] + i)->~T();
    }
    for (size_t i = 0; i < count_reserve_chunks; ++i) {
        delete[] reinterpret_cast<int8_t*>(chunks[i]);
    }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
    if (&other == this) {
        return *this;
    }
    if (begin_position.first != end_position.first) {
        for (size_t i = begin_position.second; i < CHUNK_SIZE; ++i) {
            (chunks[begin_position.first] + i)->~T();
        }
        for (size_t i = 0; i < end_position.second; ++i) {
            (chunks[end_position.first] + i)->~T();
        }
    } else {
        for (size_t i = begin_position.second; i < end_position.second; ++i) {
            (chunks[begin_position.first] + i)->~T();
        }
    }

    for (size_t i = begin_position.first + 1; i < end_position.first; ++i) {
        for (size_t j = 0; j < CHUNK_SIZE; ++j) {
            (chunks[i] + j)->~T();
        }
    }
    for (size_t i = 0; i < count_reserve_chunks; ++i) {
        delete[] reinterpret_cast<int8_t*>(chunks[i]);
    }

    _size = other._size;
    zero_of_coordinates = other.zero_of_coordinates;
    count_reserve_chunks = other.count_reserve_chunks;
    begin_position = other.begin_position;
    end_position = other.end_position;
    chunks.resize(other.count_reserve_chunks, nullptr);

    size_t i; size_t j;
    try {
        for (i = begin_position.first + 1; i < end_position.first; ++i) {
            chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
            for (j = 0; j < CHUNK_SIZE; ++j) {
                new(chunks[i] + j) T(other.chunks[i][j]);
            }
        }
    }
    catch (...) {
        for (size_t k = 0; k < i; ++k) {
            for (size_t l = 0; l < CHUNK_SIZE; ++l) {
                (chunks[k] + l)->~T();
            }
            delete[] reinterpret_cast<int8_t*>(chunks[k]);
        }

        for (size_t l = 0; l < j; ++l) {
            (chunks[i] + l)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[i]);
        throw;
    }

    if (end_position.second != 0) {
        chunks[end_position.first] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
        try {
            for (j = 0; j < end_position.second; ++j) {
                new(chunks[end_position.first] + j) T(other.chunks[i][j]);
            }
        }
        catch (...) {
            for (size_t k = 0; k < j; ++k) {
                (chunks[end_position.first] + k)->~T();
            }
            delete[] reinterpret_cast<int8_t*>(chunks[end_position.first]);
            throw;
        }
    }

    chunks[begin_position.first] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    try {
        if (begin_position.first != end_position.first) {
            for (j = begin_position.second; j < CHUNK_SIZE; ++j) {
                new(chunks[begin_position.first] + j) T(other.chunks[begin_position.first][j]);
            }
        }
        else {
            for (j = begin_position.second; j < end_position.second; ++j) {
                new(chunks[begin_position.first] + j) T(other.chunks[begin_position.first][j]);
            }
        }
    }
    catch (...) {
        for (size_t l = begin_position.second; l < j; ++l) {
            (chunks[begin_position.first] + l)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(chunks[0]);
        throw;
    }


    for (i = 0; i < begin_position.first; ++i) {
        chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
    for (i = end_position.first + 1; i < chunks.size(); ++i) {
        chunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
    return *this;
}

template <typename T>
size_t Deque<T>::size() const {
    return _size;
}

template <typename T>
size_t Deque<T>::capacity() const {
    return count_reserve_chunks * CHUNK_SIZE;
}

template <typename T>
T& Deque<T>::operator[](size_t index) {
    size_t global_index = begin_position.first * CHUNK_SIZE + begin_position.second + index;
    return chunks[global_index / CHUNK_SIZE][global_index % CHUNK_SIZE];
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const {
    size_t global_index = begin_position.first * CHUNK_SIZE + begin_position.second + index;
    return chunks[global_index / CHUNK_SIZE][global_index % CHUNK_SIZE];
}

template <typename T>
T& Deque<T>::at(size_t index) {
    size_t global_index = begin_position.first * CHUNK_SIZE + begin_position.second + index;
    size_t begin_global_index = begin_position.first * CHUNK_SIZE + begin_position.second;
    size_t end_global_index = end_position.first * CHUNK_SIZE + end_position.second;
    if (global_index < begin_global_index || global_index >= end_global_index) {
        throw std::out_of_range("out of range");
    }
    return chunks[global_index / CHUNK_SIZE][global_index % CHUNK_SIZE];
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
    size_t global_index = begin_position.first * CHUNK_SIZE + begin_position.second + index;
    size_t begin_global_index = begin_position.first * CHUNK_SIZE + begin_position.second;
    size_t end_global_index = end_position.first * CHUNK_SIZE + end_position.second;
    if (global_index < begin_global_index || global_index >= end_global_index) {
        throw std::out_of_range("out of range");
    }
    return chunks[global_index / CHUNK_SIZE][global_index % CHUNK_SIZE];
}


template <typename T>
void Deque<T>::update_of_reserved(double value) {
    count_reserve_chunks = static_cast<size_t>(floor(count_reserve_chunks * value));
    vector<T*> newchunks(count_reserve_chunks, nullptr);
    size_t count_use_chunks = end_position.first - begin_position.first;
    if (end_position.second != 0) {
        ++count_use_chunks;
    }
    size_t initial_step = (count_reserve_chunks - count_use_chunks) / 2;
    for (size_t i = 0; i < count_use_chunks; ++i) {
        newchunks[i + initial_step] = chunks[i + begin_position.first];
    }
    for (size_t i = 0; i < initial_step; ++i) {
        newchunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
    for (size_t i = count_use_chunks + initial_step; i < newchunks.size(); ++i) {
        newchunks[i] = reinterpret_cast<T*>(new uint8_t[sizeof(T) * CHUNK_SIZE]);
    }
    chunks = newchunks;
    begin_position.first = initial_step;
    end_position.first = initial_step + count_use_chunks;
    zero_of_coordinates += initial_step;
    if (end_position.second != 0) {
        --end_position.first;
    }
}

template <typename T>
void Deque<T>::expand() {
    update_of_reserved(2);
}

template <typename T>
void Deque<T>::shrink() {
    if (count_reserve_chunks <= 2) {
        return;
    }
    update_of_reserved(0.5);
}

template <typename T>
void Deque<T>::push_back(const T& value) {
    //std::cerr << "push_back\n";
    if (end_position == std::make_pair(count_reserve_chunks, static_cast<size_t>(0))) {
        expand();
    }
    if (end_position.second == CHUNK_SIZE - 1) {
        chunks[end_position.first][end_position.second] = value;
        ++end_position.first;
        end_position.second = 0;
    }
    else {
        chunks[end_position.first][end_position.second] = value;
        ++end_position.second;
    }
    ++_size;
    return;
}

template <typename T>
void Deque<T>::pop_back() {
    if (_size == 0) {
        return;
    }
    if (end_position.second == 0) {
        --end_position.first;
        end_position.second = CHUNK_SIZE - 1;
    }
    else {
        --end_position.second;
    }
    if (4 * (end_position.first - begin_position.first) <= count_reserve_chunks) {
        //shrink();
    }
    --_size;
    return;
}

template <typename T>
void Deque<T>::push_front(const T& value) {
    if (begin_position == std::make_pair(static_cast<size_t>(0), static_cast<size_t>(0))) {
        expand();
    }
    if (begin_position.second == 0) {
        --begin_position.first;
        begin_position.second = CHUNK_SIZE - 1;
        chunks[begin_position.first][begin_position.second] = value;
    }
    else {
        --begin_position.second;
        chunks[begin_position.first][begin_position.second] = value;
    }
    ++_size;
    return;
}

template <typename T>
void Deque<T>::pop_front() {
    if (_size == 0) {
        return;
    }
    if (begin_position.second == CHUNK_SIZE - 1) {
        ++begin_position.first;
        begin_position.second = 0;
    }
    else {
        ++begin_position.second;
    }
    if (4 * (end_position.first - begin_position.first) <= count_reserve_chunks) {
        //shrink();
    }
    --_size;
    return;
}
template <typename T>
void Deque<T>::insert(iterator it, const T& value) {
    size_t index_begin = end_position.first * CHUNK_SIZE + end_position.second - 1;
    size_t index_end = (it.position.first + zero_of_coordinates) * CHUNK_SIZE + it.position.second;
    if (index_end == index_begin + 1) {
        push_back(value);
        return;
    }
    push_back(chunks[index_begin / CHUNK_SIZE][index_begin % CHUNK_SIZE]);
    index_end = (it.position.first + zero_of_coordinates) * CHUNK_SIZE + it.position.second;
    for (size_t i = index_begin; i > index_end; --i) {
        chunks[i / CHUNK_SIZE][i % CHUNK_SIZE] = chunks[(i - 1) / CHUNK_SIZE][(i - 1) % CHUNK_SIZE];
    }
    chunks[index_end / CHUNK_SIZE][index_end % CHUNK_SIZE] = value;
}

template <typename T>
void Deque<T>::erase(iterator it) {
    size_t index_end = (it.position.first + zero_of_coordinates) * CHUNK_SIZE + it.position.second;
    size_t index_begin = end_position.first * CHUNK_SIZE + end_position.second - 1;
    for (size_t i = index_end; i < index_begin; ++i) {
        chunks[i / CHUNK_SIZE][i % CHUNK_SIZE] = chunks[(i + 1) / CHUNK_SIZE][(i + 1) % CHUNK_SIZE];
    }
    pop_back();
}
