cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::math
    DESCRIPTION "`math` module from dot-xx: various math utilities"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_MATH_XX "src/*.xx" )

set_source_files_properties( ${DXX_MATH_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-math STATIC )
target_sources(
    dot-xx-math PUBLIC
    FILE_SET dot_xx_math_module
    TYPE CXX_MODULES
    FILES "${DXX_MATH_XX}"
)
target_compile_features( dot-xx-math PUBLIC cxx_std_23 )
target_link_libraries(
    dot-xx-math PRIVATE
    dot-xx::assert dot-xx::cstd dot-xx::utils
)

add_library( dot-xx::math ALIAS dot-xx-math )
