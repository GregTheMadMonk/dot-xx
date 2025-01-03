import std;

import dxx.selftest;

int main(int argc, char** argv) {
    std::unordered_set<std::string> filters;
    for (std::size_t i = 1; i < argc; ++i) filters.insert(argv[i]);
    return dxx::selftest::run_all(filters);
}
