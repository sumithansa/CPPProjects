# Containers: `sk::Vector<T>` and `sk::String`

## `sk::Vector<T>`

### Storage model
Memory comes from `std::allocator<T>` as **raw, uninitialised storage**. Elements are created with
`std::construct_at` and destroyed with `std::destroy_n`, so `reserve(1000)` allocates space without
constructing 1000 objects. A naive `new T[n]` has three problems: it default-constructs every slot,
it requires `T` to be default-constructible, and it runs destructors on slots that were never used.
My first version (still in git history) had all three, plus a copy constructor that copied only
the first element.

### Growth
Capacity doubles (`0 → 1 → 2 → 4 …`), so `push_back` is amortised O(1): each element moves at most
a constant number of times on average. `next_capacity()` throws `std::length_error` instead of
letting `capacity * 2` overflow.

### Exception safety

| Operation                         | Guarantee | How                                                         |
|-----------------------------------|-----------|-------------------------------------------------------------|
| `push_back` / `emplace_back`      | strong    | build the new buffer fully, then commit with non-throwing pointer swaps |
| `reserve`, `shrink_to_fit`        | strong    | same                                                        |
| copy assignment                   | strong    | copy-and-swap                                               |
| move ops, `swap`, `clear`, `pop_back` | nothrow |                                                             |

During reallocation, elements are **moved only if `T`'s move constructor is `noexcept`**. Otherwise
they are copied. A move that throws halfway would leave some elements moved-from in the old buffer
and others half-built in the new one, with no way back. `std::vector` follows the same rule (via
`std::move_if_noexcept`), which is why move constructors should be `noexcept`. The
`ReallocationGivesStrongGuaranteeWhenCopyThrows` test injects a failure on the Nth copy and checks
that the vector is unchanged afterwards.

### Aliasing
`v.push_back(v[0])` is legal C++. If the vector reallocates, the argument points into the buffer
that is about to be freed. `emplace_back` constructs the new element in the new buffer *first*,
while the argument is still valid, and relocates the existing elements afterwards.

### Benchmark note
For `int`, `sk::Vector` matches `std::vector` within measurement noise. For `std::string` it is
about 10% slower. The reason is in libc++'s headers: `basic_string` declares itself
*trivially relocatable* (`using __trivially_relocatable = ...`), so `std::vector` moves strings
during growth with a single `memcpy` instead of calling each move constructor. Portable C++ can't
express that yet; it is the motivation for the C++26 trivial-relocatability work (P2786).

## `sk::String`

### Small-string optimisation
```
data_ ──► local_ (inline) or heap buffer
size_
union { capacity_ ; char local_[16] }   // 15 chars + '\0' fit inline
```
Most strings in real programs are short. Keeping them inside the object avoids a heap allocation
and a pointer chase. The layout matches libstdc++. `data_` always points at the characters, so
`c_str()`, `operator[]` and iteration never branch on the storage mode; only `capacity()` does.

Moving a heap string transfers the pointer. Moving an inline string has to copy the bytes, because
they live inside the source object.

### Replacing the original `customString`
| Original                                          | Now                                              |
|---------------------------------------------------|--------------------------------------------------|
| `init()` / `free()` two-phase lifetime            | RAII: constructor acquires, destructor releases  |
| `free()` called the destructor explicitly (UB when the real destructor ran later) | removed                  |
| `init()` twice leaked the first buffer            | impossible: no `init()`                          |
| public `m_string`, `m_size`                        | private; `data()`, `size()`, `view()`            |
| `compare()` returned `bool`                        | `int` (three-way) and `operator<=>`              |
| `substr` did no bounds checking                   | throws `std::out_of_range` like `std::string`    |
| logging in constructors and destructors           | removed                                          |

`assign` and `append` handle arguments that alias `*this`: `s.append(s)` copies out of the old
buffer before releasing it.
