module dxx.assert;

import dxx.selftest;
import std;

namespace {

namespace test::dxx::assert {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;
using ::dxx::assert::Failure;

const UnitTest assert{
    "assert", [] {
        try {
            ::dxx::assert::test[false];
            test(false);
        } catch (const Failure& fail) {
            // Confirm that the error message contains at least source location
            using ::dxx::selftest::check;
            const std::string_view message{ fail.what() };
            check(message.find("assert.cc:17")) != std::string_view::npos;
        }
    }
}; // <-- assert

namespace check {

using ::dxx::selftest::check;

const UnitTest direct_compare{
    "direct_compare", [] {
        try {
            ::dxx::assert::check[42] == 42;
        } catch (const Failure& fail) {
            test(false);
        }

        try {
            ::dxx::assert::check[42] == 43;
            test(false);
        } catch (const Failure& fail) {
            const std::string_view message{ fail.what() };
            check(fail.message) == "42 != 43";
        }
    }
}; // <-- direct_compare

const UnitTest conversion{
    "conversions", [] {
        try {
            ::dxx::assert::check[3.14] == 3;
            test(false);
        } catch (const Failure& fail) {
            check(fail.message) == std::format(
                "{} != {}",
                ::dxx::assert::check_print(3.14),
                ::dxx::assert::check_print(3)
            );
        }
    }
}; // <-- conversion

const UnitTest non_formattable_conv{
    "non_formattable_conv", [] {
        try {
            std::optional<int> o = std::nullopt;
            ::dxx::assert::check[o] == true;
            test(false);
        } catch (const Failure& fail) {
            // Non-formattable
            check(fail.message) == "VALUE != 1";
        }
    }
}; // <-- non_formattable_conv

} // <-- namespace check

} // <-- namespace test::dxx::assert::assert

} // <-- namespace <anonymous>
