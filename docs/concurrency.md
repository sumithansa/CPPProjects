# Concurrency: SPSC queue, thread pool, singleton

## Lock-free SPSC ring buffer (`sk::SpscQueue<T>`)

### Correctness argument
- `tail_` is written **only** by the producer and `head_` **only** by the consumer. With a single
  writer per index, no compare-and-swap is needed.
- The producer constructs the element, then does `tail_.store(t + 1, release)`. The consumer does
  `tail_.load(acquire)`. The release/acquire pair makes the element's construction *happen-before*
  the consumer reads it.
- The same pairing on `head_` tells the producer that a slot has been vacated (the element is
  destroyed before the release store).
- Indices only increase and are masked into the buffer, so "full" is `tail - head == capacity`,
  with no reserved empty slot. A 64-bit index would take centuries to wrap at 10⁹ ops/s.

ThreadSanitizer runs `ProducerConsumerTransferEveryItemInOrder` (one million items) in CI.

### Performance design
- **No false sharing.** Consumer state (`head_`, `cached_tail_`) and producer state (`tail_`,
  `cached_head_`) live on separate cache lines. Apple silicon uses 128-byte lines, so the queue
  aligns to 128 there and to 64 elsewhere.
- **Cached indices.** Each side keeps a private copy of the other side's index and re-reads the
  shared atomic only when the cached value says the queue looks full or empty.

### What the benchmark showed (Apple M1, 10M items)
| Queue            | Capacity 1024 | Capacity 65536 |
|------------------|---------------|----------------|
| `sk::SpscQueue`  | **~1.4× the mutex queue** | ~0.8× the mutex queue |
| mutex + deque    | baseline      | baseline       |

I expected a larger win, so I dug in with a probe program that counts failed push and pop calls:

1. **A busy consumer slows down the producer.** Without a pause, the consumer polled an empty
   queue about 10⁹ times per 20M items. Every poll is an acquire load of `tail_`, which pulls that
   cache line into the consumer's core. Each producer store then needs the line back: one
   coherence miss per item, roughly 60 ns on M1. Adding `sk::cpu_relax()` (x86 `PAUSE`, ARM
   `ISB`) to the spin loops cut failed polls about 10× and lifted throughput at capacity 1024.
2. **Near-empty is the worst case.** With a large ring and a consumer that keeps up, the queue
   stays nearly empty. Producer and consumer then touch *the same slot cache line* on every item.
   The cached indices can't help, because the data itself moves between cores. A smaller ring
   lets the producer fill it in bursts, so each line moves once per 16 items instead of once per
   item. This is why capacity 1024 beats 65536.
3. **macOS mutexes are cheap under low contention.** `std::mutex` on Darwin is backed by
   `os_unfair_lock`, which is fast enough that a lock-free design only clearly wins when both
   sides stay busy.

Possible next steps: batch publication (update `tail_` once per N items), and measuring on x86
Linux, where core-to-core latency is lower and `PAUSE` behaves differently. CI runs the benchmark
build on Linux so those numbers can be compared.

## Thread pool (`sk::ThreadPool`)
- `submit(f, args...)` returns a `std::future`. Exceptions thrown by the task are captured by
  `std::packaged_task` and re-thrown from `future::get()`.
- Arguments are decay-copied and passed as rvalues, the same semantics as `std::async`, so
  move-only arguments such as `std::unique_ptr` work.
- Shutdown is graceful: the destructor sets `stopping_`, wakes all workers, and each worker drains
  the queue before exiting. Submitting after shutdown throws instead of losing the task.
- `wait_idle()` uses a second condition variable. Waits always use the predicate form to handle
  spurious wake-ups.

## Singleton: why the original was a data race
```cpp
static Singleton* instance;           // plain pointer
if (instance == nullptr) {            // (1) read without the lock
    std::lock_guard lock(mtx);
    if (instance == nullptr)
        instance = new Singleton();   // (2) write under the lock
}
```
Read (1) races with write (2), which is undefined behaviour under the C++ memory model. In
practice, the compiler and the CPU (especially weakly ordered ARM) can make the pointer visible
before the constructor's writes, so another thread can use a half-built object. Double-checked
locking is only correct with `std::atomic<Singleton*>` and acquire/release ordering.

`sk::Singleton<T>` uses a function-local static instead. Since C++11 its initialisation is
guaranteed to happen exactly once ("magic statics"), and the compiler emits the atomic guard.
`SingletonTest.ConcurrentFirstAccessConstructsExactlyOnce` has 16 threads race on first access
while the constructor sleeps, then checks there was exactly one construction.
