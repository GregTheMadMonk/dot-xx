cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::assert
    DESCRIPTION "`assert` module from dot-xx: persistent and debug-only asserts"
    LANGUAGES C CXX
)

set_source_files_properties( "src/assert.xx" PROPERTIES LANGUAGE CXX )
add_library( dot-xx-assert STATIC )
target_sources(
    dot-xx-assert PUBLIC
    FILE_SET dot_xx_assert_module
    TYPE CXX_MODULES
    FILES "src/assert.xx"
)
target_compile_features( dot-xx-assert PUBLIC cxx_std_23 )

add_library( dot-xx::assert ALIAS dot-xx-assert )
