import dxx.http;
import dxx.selftest;
import std;

namespace test::dxx::http::path {

using ::dxx::selftest::UnitTest;
using ::dxx::selftest::test;

using PathTree = ::dxx::http::PathNode<int>;

const UnitTest search{
    "search", [] {
        PathTree root{ "/" };
        test(root.find("/") == nullptr);
        test(root.emplace("/", 46) == std::addressof(root));
        test(root.find("/") != nullptr);
        test(root.find("/") == std::addressof(root));
        test(root.find("/")->get_data() == 46);
        test(root.find("api1") == nullptr);
        test(root.find("api2") == nullptr);
        auto* api1 = root.emplace("/api1", 1);
        test(root.find("api1") != nullptr);
        test(root.find("/api1") == root.find("api1"));
        test(root.find("api2") == nullptr);
        test(root.find("/api2") == nullptr);
        test(root.find("api1/point") == nullptr);
        test(root.find("/api1/point") == nullptr);

        std::vector<std::string> substs;
        auto* any = root.emplace("/#/point", 42);
        test(root.find("/api1/point", &substs) != nullptr);
        test(root.find("/api1/point")->get_data() == 42);
        test(std::ranges::equal(substs, std::vector{ "api1" }));

        substs.clear();
        test(root.find("/api2/point", &substs) == root.find("/api1/point"));
        test(std::ranges::equal(substs, std::vector{ "api2" }));

        substs.clear();
        test(root.find("api1/point", &substs) == root.find("/api1/point"));
        test(std::ranges::equal(substs, std::vector{ "api1" }));

        substs.clear();
        test(root.find("api1/amogus", &substs) == nullptr);
        test(substs.empty());

        test(root.find("api3") == nullptr);

        substs.clear();
        test(root.find("api3/point", &substs)->get_data() == 42);
        test(std::ranges::equal(substs, std::vector{ "api3" }));
        substs.clear();
        test(root.find("api3/amougs", &substs) == nullptr);
        test(substs.empty());

        root.emplace("/##", 0xbeef);
        test(root.find("api3", &substs) != nullptr);
        test(root.find("api3")->get_data() == 0xbeef);
        test(std::ranges::equal(substs, std::vector{ "api3" }));

        substs.clear();
        test(root.find("api3/point", &substs) == root.find("api2/point"));
        test(std::ranges::equal(substs, std::vector{ "api3" }));
        substs.clear();
        test(root.find("api3/point2", &substs) == root.find("api3"));
        test(std::ranges::equal(substs, std::vector{ "api3/point2" }));
    }
}; // <-- search

const UnitTest substs_one{
    "substs_one", [] {
        PathTree root{ "/", 0 };
        auto* target = root.emplace("/users/#/get_name", 1);
        test(target != nullptr);
        std::vector<std::string> substs;
        test(root.find("/users/greg/get_name", &substs) == target);
        test(std::ranges::equal(substs, std::vector{ "greg" }));
    }
}; // <-- substs_one

const UnitTest substs_many{
    "substs_many", [] {
        PathTree root{ "/", 0 };
        auto* target = root.emplace("/users/#/say_to/#/##", 1);
        test(target != nullptr);
        std::vector<std::string> substs;
        test(root.find("/users/me/say_to/self/hi/me", &substs) == target);
        test(std::ranges::equal(substs, std::vector{ "me", "self", "hi/me" }));
    }
}; // <-- substs_many

const UnitTest emplace{
    "emplace", [] {
        PathTree root{ "/" };
        test(root.get_name() == "/");
        auto* api1 = root.emplace("/api1", 1);
        test(api1->get_name() == "api1");
        auto* api2_end = root.emplace("/api2/end", 2);
        test(api2_end->get_name() == "end");
        auto* api1_end = root.emplace("api1/end", 3);
        test(api1_end->get_name() == "end");
        test(api1_end->get_data() == 3);
        auto* api1_end_alt = api1->emplace("/end", 42);
        test(api1_end == api1_end_alt);
        test(api1_end->get_data() == 42);
    }
}; // <-- emplace

} // <-- namespace test::dxx::http::path
