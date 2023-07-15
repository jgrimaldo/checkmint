#include <iostream>

#include <checkmint/checkmint.hpp>

template <typename T>
concept Integral = std::is_integral_v<T>;

template <Integral T>
using NaturalNumber = cm::CheckedVar<T, ZeroOrPositive>;

template <Integral T>
using Denominator = cm::CheckedVar<T, NonZero>;

int is_prime(NaturalNumber<int> number)
{
    return number.value() % 2 == 1;
}

// An example for demonstration
auto special_divide(NaturalNumber<int> number, Denominator<int> denominator)
    -> int
{
    return number.value() / denominator.value();
}



int main() 
{
    constexpr NaturalNumber<int> n(0);
    constexpr NaturalNumber<int> d(Denominator<int>(3));
    std::cout << static_cast<Denominator<int>>(n).value() << std::endl;    

    constexpr NaturalNumber<int> m(10);
    constexpr int result = n - (m - 3) * (n + m);    
    constexpr NaturalNumber<int> calculation = result + 3;
    return 0;
}
   

