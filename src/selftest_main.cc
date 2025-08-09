import std;

import dxx.selftest;
import dxx.cstd.fixed;

int main(int argc, char** argv) {
    std::unordered_set<std::string> filters;
    for (int i = 1; i < argc; ++i) filters.insert(argv[i]);
    return dxx::selftest::run_all(filters);
}
