#include <type_traits>

#if defined(CHECKMINT_ASSERT_ON_INVARIANT_VIOLATION)
  #include <cassert>
  #define CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(cond, msg) assert(cond && msg)
#elif defined(CHECKMINT_IGNORE_ON_INVARIANT_VIOLATION)
  #define CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(cond, msg) ((void)0)
#elif defined(CHECKMINT_EXIT_ON_INVARIANT_VIOLATION)
  #define CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(cond, msg) if (!(cond)) { exit(-1); }
#else 
  #include <stdexcept>
  #define CHECKMINT_THROW_ON_INVARIANT_VIOLATION 1
  #define CHECKMINT_SIGNAL_VIOLATION_IF_FALSE(cond, msg) if (!(cond)) checkmint::invariant_violation(msg)
#endif


namespace checkmint {

#if defined(CHECKMINT_THROW_ON_INVARIANT_VIOLATION)
int invariant_violation(const std::string& msg) { throw std::logic_error(msg); } // note, not constexpr
#endif



/***
*  Forward declarations
*/
template <typename T, typename... Ts>
class CheckedVar;

template <typename... Ts, typename T>
constexpr auto validate(const T& v)
    -> CheckedVar<T, Ts...>;

template <typename... Ts, typename T>
constexpr auto check_invariants(const T& v)
    -> void;

/***
*  Contract style definitions
*/
template <typename... Ts, typename T>
constexpr auto post_condition(const T& v) 
    -> void
{
  check_invariants<Ts...>(v);
}    


template <typename... Ts, typename T>
constexpr auto pre_condition(const T& v) 
    -> void
{
  check_invariants<Ts...>(v);
}    


/***
*  Subset calculation
*/
template <typename... Ts> struct type_list {};

template <typename T, typename... Ts>
constexpr bool contains = (std::is_same_v<T, Ts> || ...);

template <typename Subset, typename Set>
struct is_subset;

template <typename... Ts, typename... Us>
struct is_subset<type_list<Ts...>, type_list<Us...>>
    : std::bool_constant<(contains<Ts, Us...> && ...)>
{};

template <typename Subset, typename Set>
concept IsSubset = is_subset<Subset, Set>::value;

/***
*  Core class
*/
template <typename T, typename... Ts>
class CheckedVar {  

public:

    constexpr CheckedVar(const T& value) 
        : m_val(value) 
    {
        check_invariants<Ts...>(m_val);
    }


    template <typename... Us>
        requires IsSubset<type_list<Us...>, type_list<Ts...>>
    constexpr explicit CheckedVar(const CheckedVar<T, Us...>& v) 
        : m_val (v.value())
    { }

    template <typename... Us>
    constexpr explicit CheckedVar(const CheckedVar<T, Us...>& v) 
        : m_val (v.value())
    { 
        check_invariants<Ts...>(m_val);
    }

    template <typename... Us>
        requires IsSubset<type_list<Us...>, type_list<Ts...>>
    constexpr explicit CheckedVar(const CheckedVar<T, Us...> && v) 
        : m_val (v.value())
    { }

    template <typename... Us>
    constexpr explicit CheckedVar(const CheckedVar<T, Us...> && v) 
        : m_val (v.value())
    { 
        check_invariants<Ts...>(m_val);    
    }    

    template <typename... Us>
        requires IsSubset<type_list<Us...>, type_list<Ts...>>
    constexpr explicit operator CheckedVar<T, Us...>() const
    {
        return CheckedVar<T, Us...>{m_val};
    }

    template <typename... Us>
    constexpr explicit operator CheckedVar<T, Us...>() const
    {
        // Avoid errors with static_assert(false,...) in clang
        static_assert(std::is_same_v<Ts..., Us...>,  "Invalid conversion requested: Cannot convert to a type with additional constraints.");
        return CheckedVar<T, Us...>(m_val);
    }    

    template <typename... Us>
        requires IsSubset<type_list<Ts...>, type_list<Us...>>    
    constexpr CheckedVar<T, Ts...>& operator=(const CheckedVar<T, Us...>& v)
    {
        m_val = v.value();
        return *this;
    }

    template <typename... Us>    
        constexpr CheckedVar<T, Ts...>& operator=(const CheckedVar<T, Us...>& v)
    {
        // Avoid errors with static_assert(false,...) in clang        
        static_assert(std::is_same_v<Ts..., Us...>, "Invalid assignment requested: Cannot assign to a type with fewer constraints.");
        return *this;
    }

    template <typename Func>
    constexpr void modify(Func && f) {
        T new_val = m_val;
        f(new_val);
        check_invariants<Ts...>(new_val);
        m_val = new_val;
    }

    template <typename Func>
    auto map(Func&& f) const {
        return CheckedVar<std::invoke_result_t<Func, T>, Ts...>(f(m_val));
    }

    constexpr const T& value() const 
        { return m_val; } 


    constexpr auto operator<=>(const T& value) const {
        return this->value() <=> value;
    }

    constexpr auto operator+(const T& value) const {
        return this->value() + value;
    }

    constexpr auto operator-(const T& value) const {
        return this->value() - value;
    }

    constexpr auto operator*(const T& value) const {
        return this->value() * value;
    }

    constexpr auto operator/(const T& value) const {
        return this->value() / value;
    }

    template <typename... Us>
    constexpr auto operator<=>(const CheckedVar<T, Us...>& other) const {
        return value() <=> other.value();
    }

    template <typename... Us>
    constexpr auto operator+(const CheckedVar<T, Us...>& v) const {
        return this->value() + v.value();
    }

    template <typename... Us>
    constexpr auto operator-(const CheckedVar<T, Us...>& v) const {
        return this->value() - v.value();
    }

    template <typename... Us>
    constexpr auto operator*(const CheckedVar<T, Us...>& v) const {
        return this->value() * v.value();
    }

    template <typename... Us>
    constexpr auto operator/(const CheckedVar<T, Us...>& v) const {
        return this->value() / v.value();
    }    

public:
    // Bypass constructor
    explicit constexpr CheckedVar(const T& value, std::true_type) : m_val(value) {}

private:
    T m_val;
};


template <typename... Ts, typename T>
constexpr auto check_invariants(const T& v) 
    -> void
{
    (Ts{}(v), ...);
}


template <typename N, typename T, typename... Ts>
constexpr auto add_unique_invariant(const CheckedVar<T, Ts...>& valid)
{
    if constexpr (!contains<N, Ts...>)
    {
        check_invariants<N>(valid.value());
        return CheckedVar<T, Ts..., N>(valid.value(), std::true_type{});
    }
    else
    {
        return CheckedVar<T, Ts...>(valid.value());
    }
}

template <typename... Ts, typename T>
constexpr auto validate(const T& v) 
    -> CheckedVar<T, Ts...>
{
    check_invariants<Ts...>(v);
    return CheckedVar<T, Ts ...>(v);
}

template <typename... Ns, typename... Ts, typename T>
constexpr auto validate(const CheckedVar<T, Ts...>& v) 
    -> CheckedVar<T, Ts..., Ns...>
{
    check_invariants<Ns...>(v.value());
    return CheckedVar<T, Ts ..., Ns...>(v.value(), std::true_type{});
}

}
