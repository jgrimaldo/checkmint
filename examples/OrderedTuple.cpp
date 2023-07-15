#include <checkmint/checkmint.hpp>

/***
* Larger example
*/
#include <tuple>

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
