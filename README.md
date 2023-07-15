# Checkmint v0.1 - Checked Variables Library for C++

Checkmint is a lightweight header-only library for C++ that provides a proof-of-concept for checked variables, allowing you to enforce constraints and ensure data integrity in your code. It offers a convenient way to incorporate compile and runtime checks and enhance design-by-contract programming.

## Features

- Define checked variables with specified constraints using a simple syntax.
- Reinforce data integrity through automatic validation.
- Supports seamless composition and conversion of checked variables.
- Lightweight and header-only, making it easy to integrate into your projects.

## Example

Validators are stateless and need to be defined following a specific convention.

All examples assume

```cpp
#include <checkmint/checkmint.hpp>
namespace cm = checkmint;
```

Multiple validators are supported 

```cpp
struct ZeroOrPositive { 
    constexpr void operator()(int v) { 
        CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(v >= 0, "Value smaller than zero"); }
};

struct Positive { 
    constexpr void operator()(int v) 
        { CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(v > 0, "Value smaller than zero"); }
};

struct NonZero { 
    constexpr void operator()(int v) { 
        CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(v != 0, "Value is zero"); }
};

int main() {
    cm::CheckedVar<int, ZeroOrPositive, Positive> valid_1(5);
}
```

A more complex example 

```cpp
#include <tuple>

#include <checkmint/checkmint.hpp>

template <typename... Ts>
struct ElementsAscending {
    constexpr void operator()(const std::tuple<Ts...>& tuple) {
        static_assert(sizeof...(Ts) >= 2, "Ordered tuple must have at least two elements");

        check_elements<0, sizeof...(Ts)>(tuple);
    }

private:
    template <size_t Index, size_t Total>
    constexpr void check_elements(const std::tuple<Ts...>& tuple) {
        if constexpr (Index + 1 < Total) {
            const auto& current = std::get<Index>(tuple);
            const auto& next = std::get<Index + 1>(tuple);
            CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(current <= next, "Tuple elements are not in ascending order");
            check_elements<Index + 1, Total>(tuple);
        }
    }
};

template <typename... Ts>
using CheckedOrderedTuple = cm::CheckedVar<std::tuple<Ts...>, ElementsAscending<Ts...>>;


int main() 
{
    constexpr CheckedOrderedTuple<int, int, int> orderedTuple({1, 4, 3}); // Compile time error
    CheckedOrderedTuple<int, int, int> orderedTuple({1, 4, 3}); // Run-time error
    CheckedOrderedTuple<int, int, int> valid_orderedTuple({1, 3, 4}); // OKAY

    return 0;
}
```
