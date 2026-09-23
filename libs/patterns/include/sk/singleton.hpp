#pragma once

namespace sk {

/// CRTP base giving Derived a single, lazily-created, thread-safe instance.
///
///   class Config : public sk::Singleton<Config> {
///       friend class sk::Singleton<Config>;
///       Config() = default;
///   };
///   Config::instance().load(...);
///
/// Thread safety comes from the language, not from a mutex: since C++11 a
/// function-local static is initialised exactly once even if several threads
/// reach it concurrently ([stmt.dcl]/4 - "magic statics"). Compilers
/// implement this with a guard variable and an acquire load on the fast path.
///
/// Why not double-checked locking with a raw pointer?
///   if (!instance) { lock; if (!instance) instance = new T; }
/// The first `if (!instance)` reads the pointer without holding the lock
/// while another thread may be writing it. That is a data race, which is
/// undefined behaviour; on weakly-ordered CPUs (ARM, POWER) a reader can also
/// see the pointer before the object's constructor writes are visible. The
/// pattern is only correct if `instance` is a std::atomic<T*> with
/// acquire/release ordering, and a function-local static does exactly that
/// for free. See docs/concurrency.md.
template <typename Derived>
class Singleton {
public:
    static Derived& instance() {
        static Derived object;
        return object;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

private:
    // Private + friend (rather than protected) so only Derived itself can
    // inherit: `class Other : Singleton<Config>` fails to compile.
    friend Derived;
    Singleton() = default;
    ~Singleton() = default;
};

} // namespace sk
