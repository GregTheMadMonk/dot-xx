import dxx.math.linalg;
import dxx.selftest;

import std;

namespace {

namespace test::dxx::math::linalg::basic_ops {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

using Vec  = std::vector<float>;
using Vec3 = std::array<float, 3>;

const UnitTest add{
    "add", [] {
        using namespace ::dxx::math::vector_operators;
        test(Vec3{ 1, 0, 0 } + Vec{ 0, 2, 1 } == Vec3{ 1, 2, 1 });
        test(
            Vec3{ 1, 0, 0 } + std::views::transform(
                Vec{ 0, 2, 1 }, [] (auto f) { return 2 * f; }
            ) == Vec3{ 1, 4, 2 }
        );

        Vec3 v1{ 1, 2, 3 };
        test(v1 + 1 == Vec3{ 2, 3, 4 });
        test(1 + v1 == Vec3{ 2, 3, 4 });
        test(
            ::dxx::math::add<Vec3>(
                std::views::transform(v1, [] (auto v) { return v * 2; }),
                1
            ) == Vec3{ 3, 5, 7 }
        );

        v1 += Vec3{ 3, 2, 1 };
        test(v1 == Vec3{ 4, 4, 4 });
        v1 += 2;
        test(v1 == Vec3{ 6, 6, 6 });
    }
}; // <-- add

const UnitTest sub{
    "sub", [] {
        using namespace ::dxx::math::vector_operators;
        test(Vec3{ 1, 0, 0 } - Vec{ 0, 2, 1 } == Vec3{ 1, -2, -1 });
        test(
            Vec3{ 1, 0, 0 } - std::views::transform(
                Vec{ 0, 2, 1 }, [] (auto f) { return 2 * f; }
            ) == Vec3{ 1, -4, -2 }
        );

        Vec3 v1{ 1, 2, 3 };
        test(v1 - 1 == Vec3{ 0,  1,  2 });
        test(1 - v1 == Vec3{ 0, -1, -2 });
        test(
            ::dxx::math::sub<Vec3>(
                std::views::transform(v1, [] (auto v) { return v * 2; }),
                1
            ) == Vec3{ 1, 3, 5 }
        );

        v1 -= Vec3{ 3, 2, 1 };
        test(v1 == Vec3{ -2, 0, 2 });
        v1 -= 4;
        test(v1 == Vec3{ -6, -4, -2 });
    }
}; // <-- sub

const UnitTest mul{
    "mul", [] {
        using namespace ::dxx::math::vector_operators;
        test(Vec3{ 1, 4, 0 } * Vec{ 4, 2, 1 } == Vec3{ 4, 8, 0 });
        test(
            Vec3{ 1, 4, 0 } * std::views::transform(
                Vec{ 4, 2, 1 }, [] (auto f) { return 2 * f; }
            ) == Vec3{ 8, 16, 0 }
        );

        Vec3 v1{ 1, 2, 3 };
        test(v1 * 3 == Vec3{ 3, 6, 9 });
        test(3 * v1 == Vec3{ 3, 6, 9 });
        test(
            ::dxx::math::mul<Vec3>(
                std::views::transform(v1, [] (auto v) { return v * 2; }),
                3
            ) == Vec3{ 6, 12, 18 }
        );

        v1 *= Vec3{ 3, 2, 1 };
        test(v1 == Vec3{ 3, 4, 3 });
        v1 *= 3;
        test(v1 == Vec3{ 9, 12, 9 });
    }
}; // <-- mul

const UnitTest div{
    "div", [] {
        using namespace ::dxx::math::vector_operators;
        test(Vec3{ 1, 4, 0 } / Vec{ 4, 2, 1 } == Vec3{ 0.25f, 2.0f, 0.0f });
        test(
            Vec3{ 1, 4, 0 } / std::views::transform(
                Vec{ 4, 2, 1 }, [] (auto f) { return 2 * f; }
            ) == Vec3{ 0.125f, 1.0f, 0.0f }
        );

        Vec3 v1{ 3, 6, 9 };
        test(v1 / 3 == Vec3{ 1, 2, 3 });
        test(
            ::dxx::math::div<Vec3>(
                std::views::transform(v1, [] (auto v) { return v * 2; }),
                3
            ) == Vec3{ 2, 4, 6 }
        );
        test(24 / Vec3{ 2, 3, 8 } == Vec3{ 12, 8, 3 });

        v1 /= Vec3{ 1, 2, 3 };
        test(v1 == Vec3{ 3, 3, 3 });
        v1 /= 3;
        test(v1 == Vec3{ 1, 1, 1 });
    }
}; // <-- div

const UnitTest neg{
    "neg", [] {
        using namespace ::dxx::math::vector_operators;
        test(-Vec3{ 1, -1, 0 } == Vec3{ -1, 1, 0 });
    }
}; // <-- neg

} // <-- test::dxx::math::linalg::basic_ops

} // <-- namespace <anonymous>
