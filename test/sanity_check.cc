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

const ::dxx::selftest::UnitTest lifetime_tester{
    "lifetime_tester", [] {
        using ::dxx::selftest::LifetimeTester;
        using ::dxx::selftest::test;

        LifetimeTester t1{};
        LifetimeTester t2{};

        test(t1.get_origin_id() != t2.get_origin_id());
        test(t1.get_times_copied() == 0);
        test(t1.get_times_moved() == 0);
        test(t2.get_times_copied() == 0);
        test(t2.get_times_moved() == 0);

        LifetimeTester t3 = t1;
        LifetimeTester t4 = std::move(t2);
        test(t1.get_origin_id() == t3.get_origin_id());
        test(t2.get_origin_id() == t4.get_origin_id());
        test(t3.get_times_copied() == 1);
        test(t3.get_times_moved() == 0);
        test(t4.get_times_copied() == 0);
        test(t4.get_times_moved() == 1);

        t4 = t3;

        test(t4.get_times_copied() == 2);
        test(t4.get_times_moved()  == 0);
    }
}; // <-- lifetime_tester

} // <-- namespace selftest

} // <-- namespace <anonymous>

} // <-- namespace test::dxx

} // <-- namespace <anonymous>
