#pragma once

#include <sk/detail/config.hpp>

#include <atomic>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace sk {

namespace detail {

struct AdoptTag {};

/// Reference counts shared by every SharedPtr/WeakPtr to one object.
///
/// Counting scheme (same as libstdc++/libc++):
///  - strong_: number of SharedPtr owners. When it reaches zero the managed
///    object is destroyed (dispose).
///  - weak_:   number of WeakPtr observers, plus one while strong_ > 0. When it
///    reaches zero the control block itself is freed (destroy).
///
/// Increments are relaxed: a new reference can only be created from an
/// existing one, so no ordering is needed. Decrements are acq_rel so that all
/// writes made through other owners happen-before the object is destroyed.
class ControlBlockBase {
public:
    ControlBlockBase() = default;
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    void add_strong() noexcept { strong_.fetch_add(1, std::memory_order_relaxed); }
    void add_weak() noexcept { weak_.fetch_add(1, std::memory_order_relaxed); }

    void release_strong() noexcept {
        if (strong_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            dispose();
            release_weak();
        }
    }

    void release_weak() noexcept {
        if (weak_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            destroy();
        }
    }

    /// Used by WeakPtr::lock(): gains a strong reference only if the object
    /// is still alive. A plain fetch_add would resurrect a dying object.
    bool try_add_strong() noexcept {
        long count = strong_.load(std::memory_order_relaxed);
        while (count != 0) {
            if (strong_.compare_exchange_weak(count, count + 1, std::memory_order_acq_rel,
                                              std::memory_order_relaxed)) {
                return true;
            }
        }
        return false;
    }

    long use_count() const noexcept { return strong_.load(std::memory_order_relaxed); }

    ControlBlockBase(ControlBlockBase&&) = delete;
    ControlBlockBase& operator=(ControlBlockBase&&) = delete;

protected:
    // Non-virtual on purpose: blocks are only ever freed by `delete this`
    // inside the final derived class (see destroy()), never through a base pointer.
    ~ControlBlockBase() = default;

private:
    virtual void dispose() noexcept = 0; // destroy the managed object
    virtual void destroy() noexcept = 0; // free this control block

    std::atomic<long> strong_{1};
    std::atomic<long> weak_{1};
};

/// Control block for a pointer adopted from the caller (two allocations).
template <typename T, typename Deleter>
class PointerControlBlock final : public ControlBlockBase {
public:
    PointerControlBlock(T* ptr, Deleter deleter) : ptr_(ptr), deleter_(std::move(deleter)) {}

private:
    void dispose() noexcept override { deleter_(ptr_); }
    void destroy() noexcept override { delete this; }

    T* ptr_;
    SK_NO_UNIQUE_ADDRESS Deleter deleter_;
};

/// Control block that stores the object inline (make_shared: one allocation).
template <typename T>
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init): storage_ is raw memory by design
class InplaceControlBlock final : public ControlBlockBase {
public:
    // storage_ is deliberately left uninitialised: the object is constructed into it.
    template <typename... Args>
    explicit InplaceControlBlock(Args&&... args) { // NOLINT(cppcoreguidelines-pro-type-member-init)
        std::construct_at(object(), std::forward<Args>(args)...);
    }

    T* object() noexcept { return std::launder(reinterpret_cast<T*>(storage_)); }

private:
    void dispose() noexcept override { std::destroy_at(object()); }
    void destroy() noexcept override { delete this; }

    alignas(T) std::byte storage_[sizeof(T)]; // NOLINT(cppcoreguidelines-pro-type-member-init)
};

} // namespace detail

template <typename T>
class WeakPtr;

/// Shared-ownership smart pointer with thread-safe reference counting.
/// As with std::shared_ptr, the *counts* are thread-safe; concurrent access
/// to the same SharedPtr instance, or to the pointee, is not.
template <typename T>
class SharedPtr {
public:
    using element_type = T;

    constexpr SharedPtr() noexcept = default;
    constexpr SharedPtr(std::nullptr_t) noexcept {} // NOLINT(google-explicit-constructor)

    template <typename U, typename Deleter = std::default_delete<U>>
        requires std::is_convertible_v<U*, T*>
    explicit SharedPtr(U* ptr, Deleter deleter = Deleter()) : ptr_(ptr) {
        try {
            control_ = new detail::PointerControlBlock<U, Deleter>(ptr, deleter);
        } catch (...) {
            deleter(ptr); // don't leak the object if the control block can't be allocated
            throw;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept : ptr_(other.ptr_), control_(other.control_) {
        if (control_ != nullptr) {
            control_->add_strong();
        }
    }

    template <typename U>
        requires std::is_convertible_v<U*, T*>
    SharedPtr(const SharedPtr<U>& other) noexcept // NOLINT(google-explicit-constructor)
        : ptr_(other.ptr_), control_(other.control_) {
        if (control_ != nullptr) {
            control_->add_strong();
        }
    }

    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)),
          control_(std::exchange(other.control_, nullptr)) {}

    template <typename U>
        requires std::is_convertible_v<U*, T*>
    // NOLINTNEXTLINE(google-explicit-constructor,cppcoreguidelines-rvalue-reference-param-not-moved)
    SharedPtr(SharedPtr<U>&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)),
          control_(std::exchange(other.control_, nullptr)) {}

    // Copy-and-swap: the temporary takes over the old state and releases it in
    // its destructor. (Also correct without the self-check, which just skips
    // two pointless atomic operations.)
    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this != &other) {
            SharedPtr(other).swap(*this);
        }
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(std::move(other)).swap(*this);
        return *this;
    }

    ~SharedPtr() {
        if (control_ != nullptr) {
            control_->release_strong();
        }
    }

    void reset() noexcept { SharedPtr().swap(*this); }

    template <typename U>
    void reset(U* ptr) {
        SharedPtr(ptr).swap(*this);
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(control_, other.control_);
    }

    T* get() const noexcept { return ptr_; }
    T& operator*() const noexcept { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    long use_count() const noexcept { return control_ != nullptr ? control_->use_count() : 0; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    friend bool operator==(const SharedPtr& lhs, const SharedPtr& rhs) noexcept {
        return lhs.ptr_ == rhs.ptr_;
    }

private:
    template <typename U>
    friend class SharedPtr;
    template <typename U>
    friend class WeakPtr;
    template <typename U, typename... Args>
    friend SharedPtr<U> make_shared(Args&&... args);

    // Adopts a reference the caller has already counted. The tag keeps overload
    // resolution from preferring the public (U*, Deleter) constructor, which
    // would otherwise be an exact match for a derived control-block pointer.
    SharedPtr(detail::AdoptTag, T* ptr, detail::ControlBlockBase* control) noexcept
        : ptr_(ptr), control_(control) {}

    T* ptr_ = nullptr;
    detail::ControlBlockBase* control_ = nullptr;
};

/// Non-owning observer that can be promoted to a SharedPtr while the object lives.
/// Typical use: breaking reference cycles (e.g. child -> parent back-pointers).
template <typename T>
class WeakPtr {
public:
    constexpr WeakPtr() noexcept = default;

    WeakPtr(const SharedPtr<T>& shared) noexcept // NOLINT(google-explicit-constructor)
        : ptr_(shared.ptr_), control_(shared.control_) {
        if (control_ != nullptr) {
            control_->add_weak();
        }
    }

    WeakPtr(const WeakPtr& other) noexcept : ptr_(other.ptr_), control_(other.control_) {
        if (control_ != nullptr) {
            control_->add_weak();
        }
    }

    WeakPtr(WeakPtr&& other) noexcept
        : ptr_(std::exchange(other.ptr_, nullptr)),
          control_(std::exchange(other.control_, nullptr)) {}

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        WeakPtr(other).swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        WeakPtr(std::move(other)).swap(*this);
        return *this;
    }

    void swap(WeakPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(control_, other.control_);
    }

    ~WeakPtr() {
        if (control_ != nullptr) {
            control_->release_weak();
        }
    }

    long use_count() const noexcept { return control_ != nullptr ? control_->use_count() : 0; }
    bool expired() const noexcept { return use_count() == 0; }

    SharedPtr<T> lock() const noexcept {
        if (control_ != nullptr && control_->try_add_strong()) {
            return SharedPtr<T>(detail::AdoptTag{}, ptr_, control_);
        }
        return SharedPtr<T>();
    }

private:
    T* ptr_ = nullptr;
    detail::ControlBlockBase* control_ = nullptr;
};

/// Allocates the object and its control block together (one allocation,
/// better cache locality) - the reason std::make_shared is preferred.
template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    auto* block = new detail::InplaceControlBlock<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(detail::AdoptTag{}, block->object(), block);
}

} // namespace sk
