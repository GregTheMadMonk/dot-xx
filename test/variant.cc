import dxx.cstd.fixed;
import dxx.selftest;
import dxx.utils;

import std;

namespace {

namespace test::dxx::utils::heap::variant {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;
using ::dxx::utils::heap::Variant;

// Test variant_alternative
static_assert(
    std::same_as<
        std::variant_alternative_t<0, Variant<int, const char>>,
        int
    >
);
static_assert(
    std::same_as<
        std::variant_alternative_t<1, Variant<int, const char>>,
        const char
    >
);
static_assert(
    std::same_as<
        std::variant_alternative_t<0, const Variant<int, const char>>,
        const int
    >
);
static_assert(
    std::same_as<
        std::variant_alternative_t<1, const Variant<int, const char>>,
        const char
    >
);
static_assert(
    std::same_as<
        std::variant_alternative_t<0, volatile Variant<int, const char>>,
        volatile int
    >
);
static_assert(
    std::same_as<
        std::variant_alternative_t<1, volatile Variant<int, const char>>,
        const volatile char
    >
);

} // <-- namespace test::dxx::utils::heap::option

} // <-- namespace <anonymous>
