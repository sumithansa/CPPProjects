// Rule of five, observed: which special member function runs for each line of main().
//
// Build and run: ./example_move_semantics
#include <cstring>
#include <iostream>
#include <utility>

class Buffer {
public:
    explicit Buffer(const char* text) : size_(std::strlen(text)), data_(new char[size_ + 1]) {
        std::memcpy(data_, text, size_ + 1);
        std::cout << "  construct   \"" << data_ << "\"\n";
    }

    Buffer(const Buffer& other) : size_(other.size_), data_(new char[size_ + 1]) {
        std::memcpy(data_, other.data_, size_ + 1);
        std::cout << "  copy-construct (allocates)\n";
    }

    // Steals the pointer; leaves the source empty but valid. noexcept matters:
    // std::vector only moves elements during reallocation if this cannot throw.
    Buffer(Buffer&& other) noexcept
        : size_(std::exchange(other.size_, 0)), data_(std::exchange(other.data_, nullptr)) {
        std::cout << "  move-construct (no allocation)\n";
    }

    // Copy-and-swap: correct for self-assignment, and if the copy throws, *this is unchanged.
    // (The original version leaked: it allocated a new buffer without freeing the old one.)
    Buffer& operator=(const Buffer& other) {
        std::cout << "  copy-assign\n";
        Buffer copy(other);
        swap(copy);
        return *this;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "  move-assign\n";
        Buffer moved(std::move(other));
        swap(moved);
        return *this;
    }

    ~Buffer() { delete[] data_; }

    void swap(Buffer& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(data_, other.data_);
    }

    const char* c_str() const { return data_ != nullptr ? data_ : "(empty)"; }

private:
    std::size_t size_;
    char* data_;
};

class Entity {
public:
    // Take by value, then move: callers passing an rvalue pay one move, lvalues one copy.
    explicit Entity(Buffer name) : name_(std::move(name)) {}
    const char* name() const { return name_.c_str(); }

private:
    Buffer name_;
};

int main() {
    std::cout << "Entity from a temporary:\n";
    Entity entity(Buffer("Sumit"));
    std::cout << "  -> " << entity.name() << "\n\n";

    std::cout << "Buffer b = a;\n";
    Buffer a("Hello");
    Buffer b = a;

    std::cout << "\nBuffer c = std::move(a);\n";
    Buffer c = std::move(a);
    std::cout << "  a is now " << a.c_str() << "\n"; // NOLINT(bugprone-use-after-move)

    std::cout << "\nb = c;\n";
    b = c;

    std::cout << "\nb = std::move(c);\n";
    b = std::move(c);
    return 0;
}
