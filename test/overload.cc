import dxx.overload;
import dxx.selftest;

import std;

namespace {

using namespace std::literals;

using dxx::selftest::test;
using dxx::selftest::unit_test;
using dxx::overload::overload;

struct S {
    auto f()      const { return "S::f()"s; }
    auto g(int i) const { return std::format("S::g({})", i); }

    auto i()       { return "S::i()"s; }
    auto j() const { return "S::j() const"s; }
    auto k() &&    { return "S::k() &&"s; }
}; // <-- struct S

auto f(const S& s, float f) { return std::format("free f({})", f); }
auto g(float f) { return std::format("free g({})", f); }

const unit_test simple{
    "/dxx/overload/simple", [] {
        const overload callback{
            [] (int i)   { return "int"s;   },
            [] (float f) { return "float"s; },
            [] (char c)  { return "char"s;  },
        };

        test("int"   == callback(10));
        test("float" == callback(10.0f));
        test("char"  == callback('c'));
    }
}; // <-- /dxx/overload/simple

const unit_test member{
    "/dxx/overload/method", [] {
        const overload callback{
            [] (int i) { return "int"s; },
            &S::f,
            &S::g,
        };

        test("int"      == callback(10));
        test("S::f()"   == callback(S{}));
        test("S::g(11)" == callback(S{}, 11));
        test("S::g(11)" == callback(S{}, 11.0));
    }
}; // <-- /dxx/overload/member

const unit_test all{
    "/dxx/overload/all", [] {
        const overload callback{
            [] (int i) { return "int"s; },
            &S::f,
            &S::g,
            f
        };

        test("int"        == callback(10));
        test("S::f()"     == callback(S{}));
        test("S::g(11)"   == callback(S{}, 11));
        test("free f(11)" == callback(S{}, 11.0f));
    }
}; // <-- /dxx/overload/all

const unit_test visit{
    "/dxx/overload/visit", [] {
        const overload callback{
            [] (int i) { return "int"s; },
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
}; // <-- /dxx/overload/visit

const unit_test value_categories{
    "/dxx/overload/value_categories", [] {
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
}; // <-- /dxx/overload/value_categories

} // <-- anonymous namespace
