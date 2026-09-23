#include <sk/string.hpp>

#include <algorithm>
#include <cstring>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace sk {
namespace {

// memmove tolerates overlapping ranges (self-assignment from a substring)
// but not a null source, which string_view permits when the size is zero.
void copy_chars(char* dest, const char* src, std::size_t count) noexcept {
    if (count != 0) {
        std::memmove(dest, src, count);
    }
}

} // namespace

// The union is initialised per mode: local_[0] here, capacity_ or local_ bytes below.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
String::String() noexcept : data_(local_), size_(0) {
    local_[0] = '\0';
}

String::String(const char* str) : String(std::string_view(str)) {}

String::String(const char* str, size_type count) : String(std::string_view(str, count)) {}

String::String(std::string_view view) // NOLINT(cppcoreguidelines-pro-type-member-init)
    : data_(local_), size_(view.size()) {
    if (size_ > kSsoCapacity) {
        data_ = new char[size_ + 1];
        capacity_ = size_;
    }
    copy_chars(data_, view.data(), size_);
    data_[size_] = '\0';
}

String::String(const String& other) : String(other.view()) {}

String::String(String&& other) noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
    : data_(local_), size_(0) {
    steal(other);
}

String& String::operator=(const String& other) {
    if (this != &other) {
        assign(other.view());
    }
    return *this;
}

String& String::operator=(String&& other) noexcept {
    if (this != &other) {
        release();
        steal(other);
    }
    return *this;
}

String::~String() {
    release();
}

char& String::at(size_type pos) {
    if (pos >= size_) {
        throw std::out_of_range("sk::String::at: index out of range");
    }
    return data_[pos];
}

const char& String::at(size_type pos) const {
    return const_cast<String&>(*this).at(pos);
}

void String::reserve(size_type new_capacity) {
    if (new_capacity <= capacity()) {
        return;
    }
    char* buffer = new char[new_capacity + 1];
    copy_chars(buffer, data_, size_ + 1);
    release();
    data_ = buffer;
    capacity_ = new_capacity;
}

void String::clear() noexcept {
    size_ = 0;
    data_[0] = '\0';
}

String& String::assign(std::string_view view) {
    const size_type count = view.size();
    if (count <= capacity()) {
        copy_chars(data_, view.data(), count); // view may alias *this
    } else {
        char* buffer = new char[count + 1];
        copy_chars(buffer, view.data(), count);
        release();
        data_ = buffer;
        capacity_ = count;
    }
    size_ = count;
    data_[size_] = '\0';
    return *this;
}

String& String::append(std::string_view view) {
    const size_type new_size = size_ + view.size();
    if (new_size <= capacity()) {
        copy_chars(data_ + size_, view.data(), view.size());
    } else {
        // Geometric growth keeps repeated appends amortised O(1). The old
        // buffer is released only after copying, because view may point into it.
        const size_type new_capacity = std::max(new_size, capacity() * 2);
        char* buffer = new char[new_capacity + 1];
        copy_chars(buffer, data_, size_);
        copy_chars(buffer + size_, view.data(), view.size());
        release();
        data_ = buffer;
        capacity_ = new_capacity;
    }
    size_ = new_size;
    data_[size_] = '\0';
    return *this;
}

void String::swap(String& other) noexcept {
    String tmp(std::move(other));
    other = std::move(*this);
    *this = std::move(tmp);
}

String String::substr(size_type pos, size_type count) const {
    if (pos > size_) {
        throw std::out_of_range("sk::String::substr: position out of range");
    }
    return String(data_ + pos, std::min(count, size_ - pos));
}

String operator+(const String& lhs, std::string_view rhs) {
    String result;
    result.reserve(lhs.size() + rhs.size());
    result.append(lhs).append(rhs);
    return result;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
    return os << str.view();
}

void String::release() noexcept {
    if (!is_local()) {
        delete[] data_;
    }
    data_ = local_;
}

// Takes other's contents, leaving it as a valid empty string. Precondition:
// *this owns no heap memory. A heap buffer is transferred; an inline one is
// copied because it lives inside the source object.
void String::steal(String& other) noexcept {
    if (other.is_local()) {
        data_ = local_;
        std::memcpy(local_, other.local_, other.size_ + 1);
    } else {
        data_ = other.data_;
        capacity_ = other.capacity_;
    }
    size_ = other.size_;
    other.data_ = other.local_;
    other.size_ = 0;
    other.local_[0] = '\0';
}

} // namespace sk
