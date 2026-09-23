// Two ways to process a parameter pack: C++11 recursion and C++17 fold expressions.
#include <iostream>
#include <string>

// C++11: peel one argument off per instantiation until the base case.
template <typename T>
T sum_recursive(T value) {
    return value;
}

template <typename T, typename... Rest>
T sum_recursive(T first, Rest... rest) {
    return first + sum_recursive(rest...);
}

// C++17: a fold expression expands the pack in one step, no recursion needed.
template <typename... Ts>
auto sum_fold(Ts... values) {
    return (values + ...);
}

// Fold over the comma operator to apply an action to every argument in order.
template <typename... Ts>
void print_all(const Ts&... values) {
    ((std::cout << values << ' '), ...);
    std::cout << '\n';
}

// sizeof... is evaluated at compile time.
template <typename... Ts>
constexpr std::size_t count_args(const Ts&...) {
    return sizeof...(Ts);
}

int main() {
    std::cout << "sum_recursive(2, 3, 4, 5, 6) = " << sum_recursive(2, 3, 4, 5, 6) << '\n';
    std::cout << "sum_fold(1.5, 2, 3)        = " << sum_fold(1.5, 2, 3) << '\n';
    print_all("mixed", 'p', "ack:", 42, 3.14, std::string("done"));
    static_assert(count_args(1, 'a', "b") == 3);
    return 0;
}
