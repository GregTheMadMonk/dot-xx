import dxx.selftest;

import std;

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

static const ::dxx::selftest::UnitTest unnamed2 = [] {
    ::dxx::selftest::test(true);
}; // <-- unnamed2

const ::dxx::selftest::UnitTest lifetime_tester{
    "lifetime_tester", [] {
        using ::dxx::selftest::LifetimeTester;
        using ::dxx::selftest::test;
        using ::dxx::selftest::check;

        LifetimeTester t1{};
        LifetimeTester t2{};

        check(t1.get_origin_id()) != t2.get_origin_id();
        check(t1.get_times_copied()) == 0uz;
        check(t1.get_times_moved()) == 0uz;
        check(t2.get_times_copied()) == 0uz;
        check(t2.get_times_moved()) == 0uz;

        LifetimeTester t3 = t1;
        LifetimeTester t4 = std::move(t2);
        check(t1.get_origin_id()) == t3.get_origin_id();
        check(t2.get_origin_id()) == t4.get_origin_id();
        check(t3.get_times_copied()) == 1uz;
        check(t3.get_times_moved()) == 0uz;
        check(t4.get_times_copied()) == 0uz;
        check(t4.get_times_moved()) == 1uz;

        t4 = t3;

        check(t4.get_times_copied()) == 2uz;
        check(t4.get_times_moved())  == 0uz;
    }
}; // <-- lifetime_tester

} // <-- namespace selftest

} // <-- namespace <anonymous>

} // <-- namespace test::dxx

} // <-- namespace <anonymous>
