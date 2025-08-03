import dxx.utils;

namespace {

template <typename...> struct S {};
template <typename... Ts> struct T {
    operator S<Ts...>() const;
};

using namespace dxx::utils;

// Only compile-time tests
static_assert(!const_vrp<int*>);
static_assert(!const_vrp<int* const>);
static_assert( const_vrp<const int* const>);
static_assert( const_vrp<const int*>);

static_assert(!const_vrp<int&>);
static_assert(!const_vrp<int&>);
static_assert( const_vrp<const int&>);
static_assert( const_vrp<const int&>);

static_assert(!const_vrp<int>);
static_assert( const_vrp<const int>);

static_assert(!is_a<T<int>, S>);
static_assert(!is_a<T<int, float>, S>);
static_assert( is_a<S<int>, S>);
static_assert( is_a<S<int, float>, S>);
static_assert(!is_a<S<int>, T>);
static_assert(!is_a<S<int, float>, T>);
static_assert( is_a<T<int>, T>);
static_assert( is_a<T<int, float>, T>);

} // <-- namespace <anonymous>
