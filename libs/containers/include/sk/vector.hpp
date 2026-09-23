#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace sk {

/// A contiguous, growable array with the same core semantics as std::vector.
///
/// Design notes (see docs/containers.md for the full write-up):
///  - Storage is raw memory from std::allocator; elements are created with
///    std::construct_at and destroyed explicitly, so capacity never
///    default-constructs objects that were not asked for.
///  - Reallocation gives the strong exception guarantee: elements are moved
///    only if T's move constructor is noexcept, otherwise they are copied
///    (the same rule std::vector follows via std::move_if_noexcept).
///  - emplace_back is safe when its argument aliases an element of this
///    vector (e.g. v.push_back(v[0])) because the new element is constructed
///    before the old buffer is released.
template <typename T>
class Vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Vector() noexcept = default;

    explicit Vector(size_type count) : Vector() {
        reserve(count);
        std::uninitialized_value_construct_n(data_, count);
        size_ = count;
    }

    Vector(size_type count, const T& value) : Vector() {
        reserve(count);
        std::uninitialized_fill_n(data_, count, value);
        size_ = count;
    }

    Vector(std::initializer_list<T> init) : Vector() {
        reserve(init.size());
        std::uninitialized_copy(init.begin(), init.end(), data_);
        size_ = init.size();
    }

    Vector(const Vector& other) : Vector() {
        reserve(other.size_);
        std::uninitialized_copy_n(other.data_, other.size_, data_);
        size_ = other.size_;
    }

    Vector(Vector&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)), size_(std::exchange(other.size_, 0)),
          capacity_(std::exchange(other.capacity_, 0)) {}

    // Copy-and-swap: if the copy throws, *this is untouched (strong guarantee).
    Vector& operator=(const Vector& other) {
        if (this != &other) {
            Vector tmp(other);
            swap(tmp);
        }
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        Vector tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    ~Vector() {
        std::destroy_n(data_, size_);
        deallocate(data_, capacity_);
    }

    // Element access
    reference operator[](size_type pos) noexcept {
        assert(pos < size_);
        return data_[pos];
    }
    const_reference operator[](size_type pos) const noexcept {
        assert(pos < size_);
        return data_[pos];
    }

    reference at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("sk::Vector::at: index out of range");
        }
        return data_[pos];
    }
    const_reference at(size_type pos) const { return const_cast<Vector&>(*this).at(pos); }

    reference front() noexcept { return (*this)[0]; }
    const_reference front() const noexcept { return (*this)[0]; }
    reference back() noexcept { return (*this)[size_ - 1]; }
    const_reference back() const noexcept { return (*this)[size_ - 1]; }
    pointer data() noexcept { return data_; }
    const_pointer data() const noexcept { return data_; }

    // Iterators (raw pointers model std::contiguous_iterator)
    iterator begin() noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator cbegin() const noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator end() const noexcept { return data_ + size_; }
    const_iterator cend() const noexcept { return data_ + size_; }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    // Capacity
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    size_type capacity() const noexcept { return capacity_; }
    static constexpr size_type max_size() noexcept {
        return std::allocator_traits<std::allocator<T>>::max_size(std::allocator<T>{});
    }

    void reserve(size_type new_capacity) {
        if (new_capacity > capacity_) {
            reallocate(new_capacity);
        }
    }

    void shrink_to_fit() {
        if (capacity_ == size_) {
            return;
        }
        if (size_ == 0) {
            deallocate(data_, capacity_);
            data_ = nullptr;
            capacity_ = 0;
            return;
        }
        reallocate(size_);
    }

    // Modifiers
    void clear() noexcept {
        std::destroy_n(data_, size_);
        size_ = 0;
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ < capacity_) {
            std::construct_at(data_ + size_, std::forward<Args>(args)...);
            return data_[size_++];
        }

        const size_type new_capacity = next_capacity();
        pointer new_data = allocate(new_capacity);
        pointer slot = new_data + size_;

        // Construct the new element first: args may refer into the old buffer.
        try {
            std::construct_at(slot, std::forward<Args>(args)...);
        } catch (...) {
            deallocate(new_data, new_capacity);
            throw;
        }

        try {
            relocate(data_, size_, new_data);
        } catch (...) {
            std::destroy_at(slot);
            deallocate(new_data, new_capacity);
            throw;
        }

        commit(new_data, new_capacity);
        return data_[size_++];
    }

    void pop_back() noexcept {
        assert(!empty());
        std::destroy_at(data_ + --size_);
    }

    iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

    iterator erase(const_iterator first, const_iterator last) {
        assert(begin() <= first && first <= last && last <= end());
        const auto offset = first - cbegin();
        const auto count = static_cast<size_type>(last - first);
        iterator dest = begin() + offset;
        if (count != 0) {
            std::move(dest + count, end(), dest);
            std::destroy_n(end() - count, count);
            size_ -= count;
        }
        return dest;
    }

    void resize(size_type count) {
        if (count < size_) {
            std::destroy_n(data_ + count, size_ - count);
        } else if (count > size_) {
            reserve(count);
            std::uninitialized_value_construct_n(data_ + size_, count - size_);
        }
        size_ = count;
    }

    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    friend void swap(Vector& lhs, Vector& rhs) noexcept { lhs.swap(rhs); }

    friend bool operator==(const Vector& lhs, const Vector& rhs) {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

private:
    static pointer allocate(size_type n) {
        return n == 0 ? nullptr : std::allocator<T>{}.allocate(n);
    }

    static void deallocate(pointer p, size_type n) noexcept {
        if (p != nullptr) {
            std::allocator<T>{}.deallocate(p, n);
        }
    }

    // Moves if that cannot throw (or copying is impossible), otherwise copies,
    // so a throwing element leaves the source buffer intact.
    static void relocate(pointer src, size_type n, pointer dest) {
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(src, n, dest);
        } else {
            std::uninitialized_copy_n(src, n, dest);
        }
    }

    size_type next_capacity() const {
        if (capacity_ > max_size() / 2) {
            throw std::length_error("sk::Vector: capacity overflow");
        }
        return capacity_ == 0 ? 1 : capacity_ * 2;
    }

    void reallocate(size_type new_capacity) {
        pointer new_data = allocate(new_capacity);
        try {
            relocate(data_, size_, new_data);
        } catch (...) {
            deallocate(new_data, new_capacity);
            throw;
        }
        commit(new_data, new_capacity);
    }

    void commit(pointer new_data, size_type new_capacity) noexcept {
        std::destroy_n(data_, size_);
        deallocate(data_, capacity_);
        data_ = new_data;
        capacity_ = new_capacity;
    }

    pointer data_ = nullptr;
    size_type size_ = 0;
    size_type capacity_ = 0;
};

} // namespace sk
