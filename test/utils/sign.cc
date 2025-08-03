import dxx.math.utils;
import dxx.selftest;

import std;

namespace {

namespace test::dxx::math::utils::sign {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

const UnitTest sign{
    "sign", [] {
        using ::dxx::math::sign;
        using enum ::dxx::math::Sign;

        test(sign(-1) == Negative);
        test(sign(1)  == Positive);
        test(sign(0)  == Zero);

        test(sign(-1.0) == Negative);
        test(sign(1.0)  == Positive);
        test(sign(0.0)  == Zero);

        test(sign(-1.0, std::greater{}) == Positive);
        test(sign(1.0, std::greater{})  == Negative);
        test(sign(0.0, std::greater{})  == Zero);

        test(sign(-1.0, std::greater{}, 1.0) == Positive);
        test(sign(1.0, std::greater{}, 1.0)  == Zero);
        test(sign(0.0, std::greater{}, 1.0)  == Positive);
    }
}; // <-- sign

} // <-- namespace test::dxx::math::utils::sign

} // <-- namespace <anonymous>
