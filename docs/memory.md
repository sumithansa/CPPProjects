# Memory: smart pointers and pool allocation

## `sk::UniquePtr<T, Deleter>`
- **Zero overhead:** the deleter is a `[[no_unique_address]]` member, so a stateless deleter takes
  no space and `sizeof(UniquePtr<T>) == sizeof(T*)` (checked with `static_assert`).
- `reset()` updates the stored pointer *before* calling the deleter, as the standard requires, so a
  deleter that reaches back into the owner sees a consistent state.
- Converting moves (`UniquePtr<Derived>` → `UniquePtr<Base>`) are constrained with `requires`.
- Custom deleters make it a general RAII handle: `examples/raii_and_object_lifetime.cpp` wraps a
  C `FILE*` with `fclose`.

Bugs fixed from the original practice version: the move assignment was declared as `operator&=`,
`release()` was `const` and couldn't compile once instantiated, and the copy constructor was
deleted on the non-const signature `unique_ptr(unique_ptr&)`.

## `sk::SharedPtr<T>` / `sk::WeakPtr<T>`

### Control block
```
strong_  number of SharedPtr owners   → 0: destroy the object (dispose)
weak_    WeakPtrs + 1 while strong_>0 → 0: free the control block (destroy)
```
- **Increments are relaxed.** A new reference is always made from an existing one, so nothing
  needs ordering.
- **Decrements are `acq_rel`.** The thread that brings the count to zero must see every write the
  other owners made through the pointer before it runs the destructor.
- `WeakPtr::lock()` uses a CAS loop that only increments a *non-zero* count. A plain `fetch_add`
  could bring an object back to life while another thread is destroying it.
- `make_shared` puts the object and its counts in **one allocation** (`InplaceControlBlock`).
  The benchmark shows it takes about half the time of `SharedPtr(new T)`, which needs two.

### Bugs fixed from the original
| Original                                                      | Effect                                  |
|---------------------------------------------------------------|-----------------------------------------|
| `operator=` overwrote the pointer without releasing the old one | leaked the previous object            |
| `int* m_reference_count`, non-atomic                          | data race when copies live on different threads |
| destructor printed `*m_reference_count` after deleting it and setting it to null | null-pointer dereference on the last release |
| `if (m_reference_count > 0)` compared the *pointer* with 0    | ill-formed (ordered pointer/zero comparison); modern compilers reject it |
| no move operations                                            | every transfer paid for a refcount round trip |

`ConcurrentCopiesKeepCountConsistent` runs under TSan in CI.

## `sk::FixedBlockPool` / `sk::ObjectPool<T>`
- One contiguous arena, divided into equal blocks aligned to `max_align_t`.
- Free blocks form an **intrusive linked list**: the first bytes of each free block hold the next
  pointer, so bookkeeping needs no extra memory.
- `allocate()` and `deallocate()` are O(1) pointer swaps. There are no system calls, no size
  classes, and no locks.
- LIFO reuse returns the most recently freed block, which is probably still in cache.
- `ObjectPool::create` returns the block to the pool if `T`'s constructor throws.

Benchmark: allocating and freeing 1,024 objects is about **15–20× faster** than `new`/`delete` on
macOS. This is why pools are common in games, network stacks and drivers, where many objects of
the same size are created and destroyed constantly.

The pool is deliberately single-threaded. A thread-safe version would use one pool per thread, or
a lock-free free list with ABA protection (tagged pointers).
