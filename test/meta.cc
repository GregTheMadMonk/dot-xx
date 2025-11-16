import dxx.utils;
import std;

using namespace dxx::utils;

static_assert(find_type_in<float, float, char, int> == 0);
static_assert(find_type_in<char,  float, char, int> == 1);
static_assert(find_type_in<int,   float, char, int> == 2);
