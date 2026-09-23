# Modern C++ Systems

[![CI](https://github.com/sumithansa/CPPProjects/actions/workflows/ci.yml/badge.svg)](https://github.com/sumithansa/CPPProjects/actions/workflows/ci.yml)
![C++20](https://img.shields.io/badge/C%2B%2B-20-blue)
![CMake](https://img.shields.io/badge/build-CMake-informational)

Standard-library building blocks (containers, smart pointers, a lock-free queue and a thread
pool) implemented from scratch in C++20, with tests, sanitizers and benchmarks against the real
`std::` versions.

The point is to show the reasoning that makes these components correct: exception-safety
guarantees, memory-ordering arguments, and object lifetimes. Each is tested under
AddressSanitizer, UndefinedBehaviorSanitizer and ThreadSanitizer, and measured instead of assumed.

## Highlights

| Component | What's interesting | Docs |
|---|---|---|
| [`sk::Vector<T>`](libs/containers/include/sk/vector.hpp) | Raw storage + `construct_at`; **strong exception guarantee** on growth (`move_if_noexcept` rule); safe `v.push_back(v[0])` during reallocation | [containers.md](docs/containers.md) |
| [`sk::String`](libs/containers/include/sk/string.hpp) | **Small-string optimisation** (15 chars inline, libstdc++ layout), self-aliasing `append`/`assign` | [containers.md](docs/containers.md) |
| [`sk::SpscQueue<T>`](libs/concurrency/include/sk/spsc_queue.hpp) | Lock-free ring buffer: acquire/release publication, cached indices, cache-line isolation (128 B on Apple silicon) | [concurrency.md](docs/concurrency.md) |
| [`sk::ThreadPool`](libs/concurrency/include/sk/thread_pool.hpp) | `submit()` → `std::future`, exception propagation, move-only args, graceful drain on shutdown | [concurrency.md](docs/concurrency.md) |
| [`sk::SharedPtr` / `WeakPtr`](libs/memory/include/sk/shared_ptr.hpp) | Atomic control block, `acq_rel` release ordering, CAS-based `lock()`, single-allocation `make_shared` | [memory.md](docs/memory.md) |
| [`sk::UniquePtr`](libs/memory/include/sk/unique_ptr.hpp) | Zero-overhead (`sizeof == sizeof(T*)`, statically asserted), custom deleters | [memory.md](docs/memory.md) |
| [`sk::FixedBlockPool`](libs/memory/include/sk/pool_allocator.hpp) | O(1) intrusive free-list allocator | [memory.md](docs/memory.md) |
| [`sk::Singleton<T>`](libs/patterns/include/sk/singleton.hpp) | Why double-checked locking on a raw pointer is a data race, and the magic-static fix | [concurrency.md](docs/concurrency.md#singleton-why-the-original-was-a-data-race) |
| [Algorithms](algorithms/include/sk/algo) | Generic iterator-based sorts (median-of-three quicksort, stable merge sort), linked list, interview problems with complexity notes | — |

## Benchmarks

Apple M1 (8 cores), AppleClang 21, `-O3`, Google Benchmark, median of 5 runs. Absolute times vary
with machine load, so results are shown as ratios, which stayed consistent across runs.

| Comparison | Result | Why |
|---|---|---|
| `ObjectPool` vs `new`/`delete` (1,024 objects) | **15–20× faster** | O(1) free-list pop/push, no allocator bookkeeping |
| `make_shared` vs `SharedPtr(new T)` | **~1.9× faster** | one allocation instead of two |
| `SpscQueue` vs mutex queue (capacity 1024, 10M items) | **~1.4× throughput** | no lock; see analysis of the cache-line ping-pong |
| `sk::Vector<int>` vs `std::vector<int>` | on par (±10%) | same growth policy |
| `sk::Vector<std::string>` vs `std::vector<std::string>` | ~10% slower | libc++ relocates `std::string` with `memcpy` ([details](docs/containers.md#benchmark-note)) |

The lock-free queue is slower than the mutex queue at capacity 65536. That turned into the most
interesting finding in this repo: [a spinning consumer steals the producer's cache
line](docs/concurrency.md#what-the-benchmark-showed-apple-m1-10m-items).

## Build and test

Requires CMake ≥ 3.25, Ninja and a C++20 compiler (GCC 12+, Clang 16+, AppleClang 15+, MSVC 19.34+).
GoogleTest and Google Benchmark are downloaded automatically via `FetchContent`.

```bash
cmake --preset debug      && cmake --build --preset debug   && ctest --preset debug
cmake --preset asan       && cmake --build --preset asan    && ctest --preset asan    # ASan + UBSan
cmake --preset tsan       && cmake --build --preset tsan    && ctest --preset tsan    # ThreadSanitizer
cmake --preset release    && cmake --build --preset release                          # + benchmarks
./build/release/benchmarks/bench_queue
```

CI builds and tests on Linux (GCC 14, Clang 18), macOS and Windows (MSVC) with warnings as errors.
It also runs both sanitizer configurations and checks clang-format and clang-tidy.

## Layout

```
libs/
  containers/    sk::Vector, sk::String                 (+ tests/)
  memory/        UniquePtr, SharedPtr/WeakPtr, pools    (+ tests/)
  concurrency/   SpscQueue, ThreadPool, cpu_relax       (+ tests/)
  patterns/      Singleton (CRTP), registry Factory     (+ tests/)
algorithms/      sorting, linked list, interview problems (+ tests/)
benchmarks/      Google Benchmark suites
examples/        small programs: move semantics, RAII, variadic templates, operator overloading …
docs/            design notes and benchmark analysis
cmake/           warnings, sanitizers, dependency setup
```

## Background

This repository started in 2024 as interview-preparation notes: one `main()` per file. In 2026 I
rebuilt it into a tested library. Several of the original versions had real bugs: a vector copy
constructor that copied only the first element, a shared pointer whose assignment leaked, a
double-checked-locking singleton with a data race, and a custom string whose `free()` called its
own destructor. Each fix now has a regression test, and the design docs explain what was wrong.
The originals are still in the git history.
