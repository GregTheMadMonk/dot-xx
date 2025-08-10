import dxx.cstd.fixed;
import dxx.math.linalg;
import std;

// Only compile-time tests
using namespace dxx::math;

static_assert(vector<const std::vector<f32>&>);
static_assert(vector<std::vector<f32>&>);
static_assert(vector<const std::vector<f32>>);
static_assert(vector<std::vector<f32>>);
static_assert(vector<std::span<f32>>);
static_assert(vector<const std::span<f32>>);
static_assert(vector<std::span<const f32>>);
static_assert(vector<const std::span<const f32>>);

static_assert(!mut_vector<const std::vector<f32>>);
static_assert( mut_vector<std::vector<f32>>);
static_assert( mut_vector<std::span<f32>>);
static_assert( mut_vector<const std::span<f32>>);
static_assert(!mut_vector<std::span<const f32>>);
static_assert(!mut_vector<const std::span<const f32>>);
