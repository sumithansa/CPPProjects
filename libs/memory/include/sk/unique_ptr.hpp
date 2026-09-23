#pragma once

#include <sk/detail/config.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace sk {

template <typename T>
struct DefaultDelete {
    constexpr DefaultDelete() noexcept = default;

    // Allows UniquePtr<Derived> -> UniquePtr<Base> conversions.
    template <typename U>
        requires std::is_convertible_v<U*, T*>
    DefaultDelete(const DefaultDelete<U>&) noexcept {} // NOLINT(google-explicit-constructor)

    void operator()(T* ptr) const noexcept {
        // Deleting an incomplete type silently skips its destructor; make it a hard error.
        static_assert(sizeof(T) > 0, // NOLINT(bugprone-sizeof-expression)
                      "cannot delete a pointer to an incomplete type");
        delete ptr;
    }
};

/// Exclusive-ownership smart pointer (single objects only; no T[] specialisation).
///
/// The deleter is stored with [[no_unique_address]] (SK_NO_UNIQUE_ADDRESS), so a stateless deleter
/// adds no space: sizeof(UniquePtr<T>) == sizeof(T*), the same zero-overhead
/// property std::unique_ptr gets from the empty-base optimisation.
template <typename T, typename Deleter = DefaultDelete<T>>
class UniquePtr {
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    constexpr UniquePtr() noexcept = default;
    constexpr UniquePtr(std::nullptr_t) noexcept {} // NOLINT(google-explicit-constructor)
    explicit UniquePtr(pointer ptr) noexcept : ptr_(ptr) {}
    UniquePtr(pointer ptr, Deleter deleter) noexcept : ptr_(ptr), deleter_(std::move(deleter)) {}

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& other) noexcept
        : ptr_(other.release()), deleter_(std::move(other.deleter_)) {}

    template <typename U, typename E>
        requires std::is_convertible_v<U*, T*> && std::is_convertible_v<E, Deleter>
    // NOLINTNEXTLINE(google-explicit-constructor,cppcoreguidelines-rvalue-reference-param-not-moved)
    UniquePtr(UniquePtr<U, E>&& other) noexcept
        : ptr_(other.release()), deleter_(std::move(other.get_deleter())) {}

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
            deleter_ = std::move(other.deleter_);
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    ~UniquePtr() { reset(); }

    pointer get() const noexcept { return ptr_; }
    Deleter& get_deleter() noexcept { return deleter_; }
    const Deleter& get_deleter() const noexcept { return deleter_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T& operator*() const noexcept { return *ptr_; }
    pointer operator->() const noexcept { return ptr_; }

    /// Gives up ownership without destroying the object.
    [[nodiscard]] pointer release() noexcept { return std::exchange(ptr_, nullptr); }

    /// Replaces the managed object. The member is updated before the old
    /// object is deleted, so a deleter that re-enters this pointer sees a
    /// consistent state (the same ordering the standard requires).
    void reset(pointer ptr = nullptr) noexcept {
        pointer old = std::exchange(ptr_, ptr);
        if (old != nullptr) {
            deleter_(old);
        }
    }

    void swap(UniquePtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(deleter_, other.deleter_);
    }

    friend bool operator==(const UniquePtr& p, std::nullptr_t) noexcept { return !p; }

private:
    pointer ptr_ = nullptr;
    SK_NO_UNIQUE_ADDRESS Deleter deleter_{};
};

template <typename T, typename... Args>
UniquePtr<T> make_unique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

} // namespace sk
