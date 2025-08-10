cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::utils
    DESCRIPTION "`utils` module from dot-xx: too misc to have its own module"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_UTILS_XX "src/*.xx" )

set_source_files_properties( ${DXX_UTILS_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-utils STATIC )
target_sources(
    dot-xx-utils PUBLIC
    FILE_SET dot_xx_utils_module
    TYPE CXX_MODULES
    FILES "${DXX_UTILS_XX}"
)
target_compile_features( dot-xx-utils PUBLIC cxx_std_23 )

if( NOT DXX_NO_CPM )
    CPMAddPackage(
        NAME "dot-xx-cstd"
        GITHUB_REPOSITORY "gregthemadmonk/dot-xx"
        GIT_TAG "cstd"
    )
endif()

target_link_libraries( dot-xx-utils PRIVATE dot-xx::cstd )

add_library( dot-xx::utils ALIAS dot-xx-utils )
