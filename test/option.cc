import dxx.cstd.fixed;
import dxx.selftest;
import dxx.utils;

import std;

namespace {

namespace test::dxx::utils::heap::option {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;
using ::dxx::utils::heap::Option;

const UnitTest comparison{
    "comparison", [] {
        using Opt = Option<int>;
        const Opt null{};
        const Opt one{1};
        const Opt two{2};

        static_assert(std::same_as<decltype(*Opt{}), int&&>);
        static_assert(std::same_as<decltype(*null),  const int&>);

        test(null == std::nullopt);
        test(std::nullopt == null);
        test(one != std::nullopt);
        test(std::nullopt != one);
        test(two != std::nullopt);
        test(std::nullopt != two);

        test(std::nullopt <= null);
        test(std::nullopt >= null);
        test(!(std::nullopt > null));
        test(!(std::nullopt < null));

        test(two > std::nullopt);
        test(two > null);
        test(two > one);
        test(!(one > one));
        test(one >= one);
        test(one <= one);
        test(!(two < one));
        test(!(two <= one));
    }
}; // <-- comparison

const UnitTest value{
    "value", [] {
        Option<int> i{};

        test(!i.has_value());
        bool exception = false;
        try {
            auto _ = i.value();
        } catch (std::bad_optional_access) {
            exception = true;
        }
        test(exception);

        test(i.value_or(12) == 12);

        i.emplace(10);
        test(i.has_value());
        test(i.value() == 10);
        test(i.value_or(12) == 10);
    }
}; // <-- value

const UnitTest lifetime{
    "lifetime", [] {
        using ::dxx::selftest::LifetimeTester;
        using Opt = Option<LifetimeTester>;

        Opt o;
        test(!o.has_value());

        o.emplace();

        test(o.has_value());
        const auto origin_id = o->get_origin_id();
        test(o->get_times_copied() == 0);
        test(o->get_times_moved() == 0);

        Opt o1 = o;
        test(o1->get_origin_id() == origin_id);
        test(o1->get_times_copied() == 1);
        test(o1->get_times_moved() == 0);

        std::swap(o, o1);
        test(o1->get_origin_id() == origin_id);
        test(o1->get_times_copied() == 0);
        test(o1->get_times_moved() == 0);
        test(o->get_origin_id() == origin_id);
        test(o->get_times_copied() == 1);
        test(o->get_times_moved() == 0);
        o.swap(o1);

        Opt o2 = std::move(o1);
        test(o2->get_origin_id() == origin_id);
        test(o2->get_times_copied() == 1);
        test(o2->get_times_moved() == 0); // Underlying value wasn't moved
                                          // Only the pointer was

        o1.emplace();
        test(o1->get_origin_id() != origin_id);
        test(o1->get_times_copied() == 0);
        test(o1->get_times_moved() == 0);

        o1 = LifetimeTester{};
        test(o1->get_origin_id() != origin_id);
        test(o1->get_times_copied() == 0);
        test(o1->get_times_moved() == 1);

        o2 = o1;
        test(o2->get_origin_id() == o1->get_origin_id());
        test(o2->get_times_copied() == 1);
        test(o2->get_times_moved() == 1);

        LifetimeTester lt;
        o2 = lt;
        test(o2->get_origin_id() == lt.get_origin_id());
        test(o2->get_times_copied() == 1);
        test(o2->get_times_moved() == 0);

        o2 = std::move(lt);
        test(o2->get_origin_id() == lt.get_origin_id());
        test(o2->get_times_copied() == 0);
        test(o2->get_times_moved() == 1);

        o1 = o2;
        test(o1->get_origin_id() == lt.get_origin_id());
        test(o1->get_times_copied() == 1);
        test(o1->get_times_moved() == 1);
    }
}; // <-- lifetime

const UnitTest transform{
    "transform", [] {
        static constexpr auto f = [] (int i) -> float { return i * 2; };

        static constexpr auto do_test =
            [] <template <typename> typename O, typename T> (O<T> opt) {
                auto o2     = opt.transform(f);
                static_assert(std::same_as<decltype(o2), O<float>>);
                test(o2.has_value());
                test(*o2 == 2.0f);

                opt.reset();
                o2 = opt.transform(f);
                test(!o2.has_value());
            }; // <-- do_test(opt)

        do_test(Option<int>{1});
        do_test(std::optional<int>{1});
    }
}; // <-- transform

const UnitTest or_else{
    "or_else", [] {
        static constexpr auto do_test =
            [] <template <typename> typename O, typename T> (O<T> opt) {
                static constexpr auto f = [] () { return O<T>{5}; };
                auto o2     = opt.or_else(f);
                static_assert(std::same_as<decltype(o2), O<int>>);
                test(o2.has_value());
                test(*o2 == 1);

                opt.reset();
                o2 = opt.or_else(f);
                test(o2.has_value());
                test(*o2 == 5);
            }; // <-- do_test(opt)

        do_test(Option<int>{1});
        do_test(std::optional<int>{1});
    }
}; // <-- or_else

const UnitTest and_then{
    "and_then", [] {
        static constexpr auto do_test =
            [] <template <typename> typename O, typename T> (O<T> opt) {
                static constexpr auto f = [] (T i) { return O<float>{i * 2}; };
                auto o2     = opt.and_then(f);
                static_assert(std::same_as<decltype(o2), O<float>>);
                test(o2.has_value());
                test(*o2 == 2);

                opt.reset();
                o2 = opt.and_then(f);
                test(!o2.has_value());
            }; // <-- do_test(opt)

        do_test(Option<int>{1});
        do_test(std::optional<int>{1});
    }
}; // <-- and_then

} // <-- namespace test::dxx::utils::heap::option

} // <-- namespace <anonymous>
