#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#endif

namespace sk {

/// Hint to the CPU that the caller is in a spin-wait loop.
///
/// Spinning on an atomic with no pause keeps pulling the cache line that the
/// other thread is trying to write, turning every one of its stores into a
/// coherence miss (see docs/concurrency.md for measurements). A pause lowers
/// the polling rate and, on SMT cores, yields pipeline resources to the
/// sibling thread.
///
///  - x86:     PAUSE (~40-140 cycles on recent Intel cores).
///  - AArch64: ISB. YIELD is architecturally a hint that most cores treat as
///             a no-op; ISB reliably stalls briefly. Rust's spin_loop() made
///             the same choice.
inline void cpu_relax() noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
#if defined(_M_X64) || defined(_M_IX86)
    _mm_pause();
#elif defined(_M_ARM64)
    __isb(_ARM64_BARRIER_SY);
#endif
#elif defined(__x86_64__) || defined(__i386__)
    __builtin_ia32_pause();
#elif defined(__aarch64__)
    asm volatile("isb" ::: "memory");
#endif
}

} // namespace sk
