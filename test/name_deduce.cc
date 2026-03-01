module dxx.selftest;

// Sanity check - compile time tests
namespace some::ns {
    [[maybe_unused]] auto l = [] {};
} // <-- namespace some:ns

namespace {
namespace test::some::ns {
    [[maybe_unused]] auto l  = [] {};
    [[maybe_unused]] auto l2 = [] {};
} // <-- namespace test::some::ns
} // <-- namespace <anonymous>

static_assert(
    !dxx::selftest::get_test_name<decltype(some::ns::l)>().has_value()
);
static_assert(
    dxx::selftest::get_test_name<decltype(test::some::ns::l)>().has_value()
);
static_assert(
    dxx::selftest::get_test_name<decltype(test::some::ns::l)>().value()
    == "/some/ns/"
);
static_assert(
    dxx::selftest::get_test_name<decltype(test::some::ns::l2)>().value()
    == "/some/ns/"
);
