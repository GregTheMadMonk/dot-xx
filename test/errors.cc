module;

#include <errno.h>

module dxx.errors;

import dxx.selftest;

namespace {

namespace test::dxx::errors {

using ::dxx::selftest::test;

const ::dxx::selftest::UnitTest literals{
    "literals", [] {
        using namespace ::dxx::errors::literals;
        using namespace std::literals;

        test("hi"_err.what() == "hi"sv);
        test("hi, {}"_err(42).what() == "hi, 42"sv);
        test("hi, {{}}"_err.what() == "hi, {}"sv);
        test("hi, {{{}}}"_err(42).what() == "hi, {42}"sv);
        test("hi, {{{{}}}}"_err.what() == "hi, {{}}"sv);

        errno = EIO;

        test("hi"_errno.what() == "hi (errno 5: Input/output error)"sv);
        test("{}"_errno(42).what() == "42 (errno 5: Input/output error)"sv);
    }
}; // <-- literals

} // <-- namespace test::dxx::errors

} // <-- namespace <anonymous>
