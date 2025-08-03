import dxx.math.linalg;
import dxx.selftest;

import dxx.cstd.fixed;
import std;

namespace {

namespace test::dxx::math::linalg::cross {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

namespace stdr = std::ranges;

using Vec  = std::vector<f32>;
using Vec3 = std::array<f32, 3>;

const UnitTest cross{
    "cross", [] {
        using ::dxx::math::cross;
        test(
            stdr::equal(
                cross(Vec{ 1, 1, 0 }, Vec{ 1, 1, 0 }),
                Vec{ 0, 0, 0 }
            )
        );
        test(
            stdr::equal(
                cross<Vec3>(Vec{ 1, 1, 0 }, Vec{ 1, 1, 0 }),
                Vec3{ 0, 0, 0 }
            )
        );

        test(
            stdr::equal(
                cross(Vec3{ 1, 0, 0 }, Vec3{ 0, 1, 0 }), Vec3{ 0, 0, 1 }
            )
        );

        test(
            stdr::equal(
                cross(Vec3{ 0, 1, 0 }, Vec3{ 1, 0, 0 }), Vec3{ 0, 0, -1 }
            )
        );
    }
}; // <-- cosnt UnitTest cross

} // <-- namespace test::dxx::math::linalg::cross

} // <-- namespace <anonymous>
