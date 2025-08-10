import dxx.cstd.fixed;
import dxx.selftest;
import dxx.utils;

import std;

namespace {

namespace test::dxx::utils::ranges {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

namespace stdr = std::ranges;

using ::dxx::utils::range;

const UnitTest range_to{
    "range_to", [] {
        test(
            stdr::equal(range(5), std::vector{ 0, 1, 2, 3, 4 })
        );
    }
}; // <-- range_to

const UnitTest range_from_to{
    "range_from_to", [] {
        test(
            stdr::equal(range(5, 10), std::vector{ 5, 6, 7, 8, 9 })
        );
    }
}; // <-- range_from_to

const UnitTest range_from_to_step{
    "range_from_to_step", [] {
        test(
            stdr::equal(
                range(50, 99uz, 7),
                std::vector{ 50, 57, 64, 71, 78, 85, 92 }
            )
        );
    }
}; // <-- range_from_to_step

const UnitTest range_float{
    "range_float", [] {
        std::vector<f32> etalon;
        for (f32 f = -1.0f; f <= 1.0f; f += 0.1f) {
            etalon.push_back(f);
        }
        test(
            stdr::equal(
                range(-1.0f, 1.1f, 0.1f),
                etalon
            )
        );
    }
}; // <-- range_float

const UnitTest as{
    "as", [] {
        using ::dxx::utils::as;
        // operator== also checks that the types are the same :)
        test((range(5) | as<std::vector>) == std::vector{ 0, 1, 2, 3, 4 });
    }
}; // <-- as

} // <-- namespace test::dxx::utils::ranges

} // <-- namespace <anonymous>
