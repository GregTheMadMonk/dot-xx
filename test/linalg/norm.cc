import dxx.math.linalg;
import dxx.selftest;

import dxx.cstd.fixed;
import std;

namespace {

namespace test::dxx::math::linalg::norm {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

using Vec = std::vector<f32>;

namespace norms = ::dxx::math::norms;

using ::dxx::math::norm;
using ::dxx::math::sq_norm;

const UnitTest euclidean{
    "euclidean", [] {
        test(sq_norm(Vec{ 3, 4 }) == 25);
        test(norm(Vec{ 3, 4 }) == 5);

        test(sq_norm(Vec{ 1, 1, 1, 1 }) == 4);
        test(norm(Vec{ 1, 1, 1, 1 }) == 2);
    }
}; // <-- euclidean

const UnitTest one{
    "one", [] {
        test(sq_norm<norms::p_one>(Vec{ 3, 4 }) == 49);
        test(norm<norms::p_one>(Vec{ 3, 4 }) == 7);

        test(sq_norm<norms::p_one>(Vec{ 1, 1, 1, 1 }) == 16);
        test(norm<norms::p_one>(Vec{ 1, 1, 1, 1 }) == 4);
    }
}; // <-- one

const UnitTest infinity{
    "infinity", [] {
        test(sq_norm<norms::infinity>(Vec{ 3, 4 }) == 16);
        test(norm<norms::infinity>(Vec{ 3, 4 }) == 4);

        test(sq_norm<norms::infinity>(Vec{ 1, 1, 1, 1 }) == 1);
        test(norm<norms::infinity>(Vec{ 1, 1, 1, 1 }) == 1);
    }
}; // <-- infinity

const UnitTest distance{
    "distance", [] {
        using ::dxx::math::dist;
        using ::dxx::math::sq_dist;

        const Vec a{ 1, 2, 3 };
        const Vec b{ 3, 4, 6 };

        const Vec d{ -2, -2, -3 };

        std::println("{} {}", sq_norm(d), sq_dist(a, b));

        test(sq_norm(d) == sq_dist(a, b));
        test(sq_norm<norms::p_one>(d) == sq_dist<norms::p_one>(a, b));
        test(sq_norm<norms::infinity>(d) == sq_dist<norms::infinity>(a, b));
    }
}; // <-- distance

} // <-- test::dxx::math::linalg::norm

} // <-- namespace <anonymous>
