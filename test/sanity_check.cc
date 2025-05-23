import dxx.selftest;

namespace test::dxx::selftest {

static const ::dxx::selftest::UnitTest sanity_check{
    "sanity_check", [] {
        ::dxx::selftest::test(true);
    }
}; // <-- sanity_check

} // <-- namespace test::dxx::selftest

namespace {

namespace test::dxx {

namespace {

namespace selftest {

static const ::dxx::selftest::UnitTest unnamed = [] {
    ::dxx::selftest::test(true);
}; // <-- unnamed

} // <-- namespace selftest

} // <-- namespace <anonymous>

} // <-- namespace test::dxx

} // <-- namespace <anonymous>
