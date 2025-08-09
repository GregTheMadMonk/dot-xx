cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::selftest
    DESCRIPTION "`selftest` module from dot-xx: unit testing"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_SELFTEST_XX "src/*.xx" )

set_source_files_properties( ${DXX_SELFTEST_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-selftest STATIC )
target_sources(
    dot-xx-selftest PUBLIC
    FILE_SET dot_xx_selftest_module
    TYPE CXX_MODULES
    FILES "${DXX_SELFTEST_XX}"
)
target_compile_features( dot-xx-selftest PUBLIC cxx_std_23 )

if( NOT DXX_NO_CPM )
    CPMAddPackage(
        NAME "dot-xx-assert"
        GITHUB_REPOSITORY "gregthemadmonk/dot-xx"
        GIT_TAG "assert"
    )
    CPMAddPackage(
        NAME "dot-xx-cstd"
        GITHUB_REPOSITORY "gregthemadmonk/dot-xx"
        GIT_TAG "cstd"
    )
endif()

target_link_libraries( dot-xx-selftest PRIVATE dot-xx::assert dot-xx::cstd )

add_library( dot-xx::selftest ALIAS dot-xx-selftest )

add_library( dot-xx-selftest-main STATIC src/selftest_main.cc )
target_compile_features( dot-xx-selftest-main PUBLIC cxx_std_23 )
target_link_libraries( dot-xx-selftest-main PUBLIC dot-xx-selftest )
add_library( dot-xx::selftest-main ALIAS dot-xx-selftest-main )
