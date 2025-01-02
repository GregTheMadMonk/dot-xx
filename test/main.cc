import std;

import dxx.assert;
import dxx.cstd.compat;
import dxx.selftest;

int main(int argc, char** argv) {
    std::println("Unit tests rely on `assert`, `cstd` and `selftest` modules");
    std::println("This is a sanity check of these modules here");
    std::println(
        "Incorrect output here indicates that unit tests may also be wrong"
    );

    std::println("Should see `Ok` in stderr here:");
    try {
        dxx::assert::always(false);
    } catch (const dxx::assert::failure&) {
        std::println(stderr, "Ok");
    }

    std::println(
        "Should see `Ok` in stderr here only of compiled in Debug mode:"
    );
    try {
        dxx::assert::debug(false);
    } catch (const dxx::assert::failure&) {
        std::println(stderr, "Ok");
    }

    std::unordered_set<std::string> filters;
    for (std::size_t i = 1; i < argc; ++i) filters.insert(argv[i]);
    return dxx::selftest::run_all(filters);
}
