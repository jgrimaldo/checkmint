# Checkmint - Checked Variables Library for C++

Checkmint is a lightweight header-only library for C++ that provides a proof-of-concept for checked variables, allowing you to enforce constraints and ensure data integrity in your code. It offers a convenient way to incorporate compile and runtime checks and enhance design-by-contract programming.

## Features

- Define checked variables with specified constraints using a simple syntax.
- Reinforce data integrity through automatic validation.
- Supports seamless composition and conversion of checked variables.
- Lightweight and header-only, making it easy to integrate into your projects.


## Example

Validators are stateless and need to be defined following a specific convention

```cpp
#include <checkmint/checkmint.hpp>

struct Positive {
    constexpr void operator()(int v) {
        CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(v > 0, "Value must be positive");
    }
};

using PositiveInt = checkmint::CheckedVar<int, Positive>;

void test(PositiveInt i)
{
}

int main() {
    PositiveInt num(42);
    PositiveInt invalidNum(-10);  // Throws an exception due to invariant violation
    test(-1);
    return 0;
}
```
