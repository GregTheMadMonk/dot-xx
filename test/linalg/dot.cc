import dxx.math.linalg;
import dxx.selftest;

import std;

namespace {

namespace test::dxx::math::linalg::dot {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

struct S {
    int val;
    operator int() const { return this->val; }
};

const UnitTest dot{
    "dot", [] {
        using ::dxx::math::dot;
        test(dot(std::vector{ 1, 2, 3 }, std::vector{ 4, 5, 6 }) == 32);
        test(dot(std::vector{ 2 }, std::vector{ S{4} }) == 8);

        test(dot(std::vector{ 2 }, std::vector{ 1.5f }) == 2);
        test(dot(std::vector{ 2.0f }, std::vector{ 1.5f }) == 3);
    }
}; // <-- dot

} // <-- namespace test::dxx::math::linalg::dot

} // <-- namespace <anonymous>
