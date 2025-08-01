cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::errors
    DESCRIPTION "`errors` module from dot-xx: exceptions and errors"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_ERRORS_XX "src/*.xx" )

set_source_files_properties( ${DXX_ERRORS_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-errors STATIC )
target_sources(
    dot-xx-errors PUBLIC
    FILE_SET dot_xx_errors_module
    TYPE CXX_MODULES
    FILES "${DXX_ERRORS_XX}"
)
target_compile_features( dot-xx-errors PUBLIC cxx_std_23 )

add_library( dot-xx::errors ALIAS dot-xx-errors )
