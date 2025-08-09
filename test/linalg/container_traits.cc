module dxx.math.linalg;

import dxx.utils;
import std;

import :container_traits;

// Only compile-time tests
namespace dxx::math::container_traits {

namespace ad = autodetect;
namespace sz = size;

static_assert(utils::declares<sz::Dynamic,      ad::size<std::vector<int>>>);
static_assert(utils::declares<sz::Static,       ad::size<std::array<int, 3>>>);
static_assert(utils::declares<sz::RuntimeFixed, ad::size<std::span<int>>>);
static_assert(utils::declares<sz::Static,       ad::size<std::span<int, 3>>>);

// Check that spans aren't constructible as return values
static_assert( constructible<std::vector<int>>);
static_assert( constructible<std::array<int, 3>>);
static_assert(!constructible<std::span<int>>);

} // <-- namespace dxx::math::container_traits
