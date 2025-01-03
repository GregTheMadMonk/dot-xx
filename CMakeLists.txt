cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::overload
    DESCRIPTION "`overloa` module from dot-xx: overload pattern"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_OVERLOAD_XX "src/*.xx" )

set_source_files_properties( ${DXX_OVERLOAD_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-overload STATIC )
target_sources(
    dot-xx-overload PUBLIC
    FILE_SET dot_xx_overload_module
    TYPE CXX_MODULES
    FILES "${DXX_OVERLOAD_XX}"
)
target_compile_features( dot-xx-overload PUBLIC cxx_std_23 )

add_library( dot-xx::overload ALIAS dot-xx-overload )
