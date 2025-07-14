cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

# Skip forcing libc++ and CMAKE_EXPERIMENTAL_CXX_IMPORT_STD here for
# forward-compatibility. Will be set in the project that uses the module if
# needed

project(
    dot-xx::http
    DESCRIPTION "`http` module from dot-xx: http server utilities"
    LANGUAGES C CXX
)

file( GLOB_RECURSE DXX_HTTP_SRC_XX "src/*.xx" )
file( GLOB_RECURSE DXX_HTTP_SRC_CC "src/*.cc" )

set_source_files_properties( ${DXX_HTTP_SRC_XX} PROPERTIES LANGUAGE CXX )
add_library( dot-xx-http STATIC ${DXX_HTTP_SRC_CC} )
target_sources(
    dot-xx-http PUBLIC
    FILE_SET dot_xx_http_module
    TYPE CXX_MODULES
    FILES ${DXX_HTTP_SRC_XX}
)
target_compile_features( dot-xx-http PUBLIC cxx_std_23 )
target_link_libraries(
    dot-xx-http PRIVATE
    dot-xx::assert dot-xx::cstd dot-xx::overload dot-xx::selftest
)

add_library( dot-xx::http ALIAS dot-xx-http )

if( DXX_SELFTEST )
    file( GLOB_RECURSE DXX_HTTP_DEMO_SRC_CC "example/*.cc" )
    add_executable( http_example ${DXX_HTTP_DEMO_SRC_CC} )
    target_link_libraries( http_example PRIVATE dot-xx::http )
    target_compile_features( http_example PRIVATE cxx_std_23 )
endif()
