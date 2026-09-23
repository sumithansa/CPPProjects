#pragma once

#include <compare>
#include <cstddef>
#include <iosfwd>
#include <string_view>

namespace sk {

/// A null-terminated, owning character string with small-string optimisation.
///
/// Strings of up to kSsoCapacity characters live inside the object itself;
/// longer strings are stored on the heap. The layout mirrors libstdc++:
///
///   data_ ──► either local_ (inline buffer) or a heap allocation
///   size_     number of characters, excluding the terminator
///   union { capacity_ (heap mode) | local_[16] (inline mode) }
///
/// Every mutating operation keeps the invariant data_[size_] == '\0', so
/// c_str() is always O(1). All operations that allocate provide the strong
/// exception guarantee.
class String {
public:
    using size_type = std::size_t;
    using iterator = char*;
    using const_iterator = const char*;

    static constexpr size_type npos = static_cast<size_type>(-1);
    static constexpr size_type kSsoCapacity = 15;

    String() noexcept;
    String(const char* str);       // NOLINT(google-explicit-constructor): mirrors std::string
    String(std::string_view view); // NOLINT(google-explicit-constructor)
    String(const char* str, size_type count);

    String(const String& other);
    String(String&& other) noexcept;
    String& operator=(const String& other);
    String& operator=(String&& other) noexcept;
    ~String();

    // Access
    const char* c_str() const noexcept { return data_; }
    const char* data() const noexcept { return data_; }
    char* data() noexcept { return data_; }
    char& operator[](size_type pos) noexcept { return data_[pos]; }
    const char& operator[](size_type pos) const noexcept { return data_[pos]; }
    char& at(size_type pos);
    const char& at(size_type pos) const;
    std::string_view view() const noexcept { return {data_, size_}; }
    operator std::string_view() const noexcept { return view(); } // NOLINT

    iterator begin() noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    iterator end() noexcept { return data_ + size_; }
    const_iterator end() const noexcept { return data_ + size_; }

    // Capacity
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    size_type size() const noexcept { return size_; }
    size_type length() const noexcept { return size_; }
    size_type capacity() const noexcept { return is_local() ? kSsoCapacity : capacity_; }
    bool is_small() const noexcept { return is_local(); }
    void reserve(size_type new_capacity);

    // Modifiers
    void clear() noexcept;
    String& assign(std::string_view view);
    String& append(std::string_view view);
    String& operator+=(std::string_view view) { return append(view); }
    void push_back(char ch) { append(std::string_view(&ch, 1)); }
    void swap(String& other) noexcept;

    // Operations
    String substr(size_type pos = 0, size_type count = npos) const;
    int compare(std::string_view other) const noexcept { return view().compare(other); }
    size_type find(std::string_view needle, size_type pos = 0) const noexcept {
        return view().find(needle, pos);
    }

    friend bool operator==(const String& lhs, std::string_view rhs) noexcept {
        return lhs.view() == rhs;
    }
    friend std::strong_ordering operator<=>(const String& lhs, std::string_view rhs) noexcept {
        return lhs.view().compare(rhs) <=> 0;
    }
    friend String operator+(const String& lhs, std::string_view rhs);
    friend std::ostream& operator<<(std::ostream& os, const String& str);

private:
    bool is_local() const noexcept { return data_ == local_; }
    void release() noexcept;
    void steal(String& other) noexcept;

    char* data_;
    size_type size_;
    union {
        size_type capacity_;
        char local_[kSsoCapacity + 1];
    };
};

inline void swap(String& lhs, String& rhs) noexcept {
    lhs.swap(rhs);
}

} // namespace sk
