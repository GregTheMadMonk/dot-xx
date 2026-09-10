module dxx.utils;

import std;

namespace {

using namespace dxx::utils;
using namespace std::literals;

static_assert(FixedString{"hello"}.size() == 5);
static_assert(FixedString{"hello"}.view() == "hello"sv);
static_assert(FixedString{"hello"}.string() == "hello"s);

template <FixedString fs> constexpr auto literal_with_extra_steps = fs.view();

static_assert(literal_with_extra_steps<"hi"> == "hi");


} // <-- namespace <anonymous>
