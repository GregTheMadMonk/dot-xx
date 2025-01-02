cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::cstd
    DESCRIPTION "`cstd` module from dot-xx: C standard library exports"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_CSTD_XX "src/*.xx" )

set_source_files_properties( ${DXX_CSTD_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-cstd STATIC )
target_sources(
    dot-xx-cstd PUBLIC
    FILE_SET dot_xx_cstd_module
    TYPE CXX_MODULES
    FILES "${DXX_CSTD_XX}"
)
target_compile_features( dot-xx-cstd PUBLIC cxx_std_23 )

add_library( dot-xx::cstd ALIAS dot-xx-cstd )
