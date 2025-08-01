import dxx.overload;
import dxx.selftest;

import std;

namespace {

namespace test::dxx::overload {

using namespace std::literals;

using ::dxx::selftest::test;
using ::dxx::selftest::UnitTest;
using ::dxx::overload::overload;

struct S {
    auto f()      const { return "S::f()"s; }
    auto g(int i) const { return std::format("S::g({})", i); }

    auto i()       { return "S::i()"s; }
    auto j() const { return "S::j() const"s; }
    auto k() &&    { return "S::k() &&"s; }
}; // <-- struct S

auto f(const S&, float f) { return std::format("free f({})", f); }
auto g(float f) { return std::format("free g({})", f); }

const UnitTest simple{
    "simple", [] {
        const overload callback{
            [] (int)   { return "int"s;   },
            [] (float) { return "float"s; },
            [] (char)  { return "char"s;  },
        };

        test("int"   == callback(10));
        test("float" == callback(10.0f));
        test("char"  == callback('c'));
    }
}; // <-- simple

const UnitTest member{
    "method", [] {
        const overload callback{
            [] (int) { return "int"s; },
            &S::f,
            &S::g,
        };

        test("int"      == callback(10));
        test("S::f()"   == callback(S{}));
        test("S::g(11)" == callback(S{}, 11));
        test("S::g(11)" == callback(S{}, 11.0));
    }
}; // <-- member

const UnitTest all{
    "all", [] {
        const overload callback{
            [] (int) { return "int"s; },
            &S::f,
            &S::g,
            f
        };

        test("int"        == callback(10));
        test("S::f()"     == callback(S{}));
        test("S::g(11)"   == callback(S{}, 11));
        test("free f(11)" == callback(S{}, 11.0f));
    }
}; // <-- all

const UnitTest visit{
    "visit", [] {
        const overload callback{
            [] (int) { return "int"s; },
            &S::f,
            &g
        };

        std::variant<int, S, float> v;

        v = 10;
        test("int" == std::visit(callback, v));

        v = S{};
        test("S::f()" == std::visit(callback, v));

        v = 10.0f;
        test("free g(10)" == std::visit(callback, v));
    }
}; // <-- visit

const UnitTest value_categories{
    "value_categories", [] {
        const overload callback{
            &S::i,
            &S::j,
            &S::k,
        };

        S s{};
        const S cs{};

        test("S::i()"       == callback(s));
        test("S::j() const" == callback(cs));
        test("S::k() &&"    == callback(S{}));
    }
}; // <-- value_categories

const UnitTest pipe_operator{
    "pipe_operator", [] {
        const overload callback{
            &S::i,
            &S::j,
            &S::k,
        };

        std::variant<S> s        = S{};
        const std::variant<S> cs = S{};

        test("S::i()"       == (s                 | callback));
        test("S::j() const" == (cs                | callback));
        test("S::k() &&"    == (std::variant<S>{} | callback));
    }
}; // <-- pipe_operator

} // <-- namespace test::dxx::overload

} // <-- anonymous namespace
