#include <checkmint/checkmint.hpp>

/**
* Ordered Pair example
*/ 
struct OrderedPair {
    int first;
    int second;
};

struct FirstSmallerThanSecond {
    constexpr void operator()(const OrderedPair& p) {
        CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(
            p.first < p.second, 
           "First number is not smaller than the second");
    }
};

using ValidOrderedPair = cm::CheckedVar<OrderedPair, FirstSmallerThanSecond>;


int main() 
{
    OrderedPair p1 = {1, 2};
    ValidOrderedPair vp1(p1);  // OK
    constexpr ValidOrderedPair vp2({3, 2});  // NOT OK  
    vp1.modify([](OrderedPair& v) { v.first = 3; });
    
    return 0;
}
    
