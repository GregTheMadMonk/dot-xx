import dxx.utils;
import dxx.selftest;

import std;

namespace {

namespace test::dxx::utils::defer {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

using ::dxx::utils::Defer;

const UnitTest defer{
    "defer", [] {
        int i = 10;
        {
            Defer _ = [&i] { i = 0; };
            i = 20;
        }
        test(i == 0);
    }
}; // <-- defer

const UnitTest defer_order{
    "defer_order", [] {
        int i = 10;
        {
            // C++26's allows to use placeholder `_` for both here
            Defer a = [&i] { i = 0; };
            Defer b = [&i] { i = 15; };
            i = 20;
        }
        test(i == 0);
    }
}; // <-- defer_order

} // <-- namespace test::dxx::utils::defer

} // <-- namespace <anonymous>
