import dxx.math.utils;
import dxx.selftest;

import std;

namespace {

namespace test::dxx::math::utils {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

unsigned proj(unsigned num) { return num % 2; }
unsigned less(unsigned a, unsigned b) { return proj(a) < proj(b); }
unsigned diff(unsigned a, unsigned b) { return 2 * proj(a) - 3 * proj(b); }

const UnitTest abs_diff{
    "abs_diff", [] {
        unsigned a = 2;
        unsigned b = 5;

        using ::dxx::math::abs_diff;
        test(abs_diff(a, b) == 3);
        test(abs_diff(b, a) == 3);

        test(abs_diff(a, b, less, diff) == 2);
        test(abs_diff(b, a, less, diff) == 2);

    }
}; // <-- abs_diff

} // <-- namespace test::dxx::math::utils

} // <-- namespace <anonymous>
