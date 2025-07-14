cmake_minimum_required( VERSION 3.30 FATAL_ERROR )

project(
    dot-xx
    DESCRIPTION "A personal collection of small C++ modules"
    LANGUAGES C CXX
)

option( DXX_LOCAL    "Specify a local directory to load modules from" OFF )
option( DXX_NO_CPM   "Don't try to use CPM.cmake"                     OFF )
option( DXX_SELFTEST "Test the modules"                               OFF )
option( DXX_DOXYGEN  "Generate docs with doxygen"                     OFF )

option( DXX_WITH_HTTP "Include dxx.http module" OFF )

if( DXX_SELFTEST )
    set( CMAKE_CXX_MODULE_STD ON )
endif()

if( DXX_LOCAL )
    include( FetchContent )
    set( DXX_NO_CPM ON )
endif() # DXX_LOCAL

if( NOT DXX_NO_CPM )
    include( CPM.cmake/cmake/CPM.cmake )
endif() # NOT CXX_NO_CPM

function( fetch_module MOD_NAME )
    if( DXX_LOCAL )
        FetchContent_Declare(
            "dot-xx-${MOD_NAME}"
            SOURCE_DIR "${DXX_LOCAL}/${MOD_NAME}"
        )
        FetchContent_MakeAvailable( "dot-xx-${MOD_NAME}" )
    elseif( NOT DXX_NO_CPM )
        CPMAddPackage(
            NAME "dot-xx-${MOD_NAME}"
            GITHUB_REPOSITORY "gregthemadmonk/dot-xx"
            GIT_TAG "${MOD_NAME}"
        )

        set(
            "${CPM_LAST_PACKAGE_NAME}_SOURCE_DIR" 
            "${${CPM_LAST_PACKAGE_NAME}_SOURCE_DIR}"
            PARENT_SCOPE
        )
    endif()

    target_link_libraries( dot-xx-all INTERFACE "dot-xx::${MOD_NAME}" )

    list( APPEND DXX_MODULES "${MOD_NAME}" )
    set( DXX_MODULES "${DXX_MODULES}" PARENT_SCOPE )
endfunction() # fetch_module( MOD_NAME )

add_library( dot-xx-all INTERFACE )
add_library( dot-xx::all ALIAS dot-xx-all )

fetch_module( assert   )
fetch_module( cstd     )
if( DXX_WITH_HTTP )
    fetch_module( http     )
endif()
fetch_module( overload )
fetch_module( selftest )

message( STATUS "dot-xx modules: ${DXX_MODULES}" )

if( DXX_SELFTEST )
    if( DXX_NO_CPM AND NOT DXX_LOCAL )
        message(
            FATAL_ERROR
            "Requested unit test build but neither CPM is used "
            "nor DXX_LOCAL defined"
        )
    endif()

    foreach( DXX_MODULE IN LISTS DXX_MODULES )
        if( DXX_LOCAL )
            # Get from local storage
            set( DXX_TST_DIR "${DXX_LOCAL}/${DXX_MODULE}/test" )
        else()
            # Get from CPM
            set( DXX_TST_DIR "${dot-xx-${DXX_MODULE}_SOURCE_DIR}/test" )
        endif()

        file( GLOB_RECURSE DXX_MOD_TST_FILES "${DXX_TST_DIR}/*.cc" )
        list( APPEND DXX_TST_FILES ${DXX_MOD_TST_FILES} )
    endforeach() # DXX_MODULE IN LISTS DXX_MODULES

    message( STATUS "Test files: ${DXX_TST_FILES}" )
    add_executable( selftest test/main.cc ${DXX_TST_FILES} )
    target_link_libraries( selftest PUBLIC dot-xx::all )
endif()

if( DXX_DOXYGEN )
    if( DXX_NO_CPM AND NOT DXX_LOCAL )
        message(
            FATAL_ERROR
            "Requested Doxygen documentation generation but neither CPM is used"
            " not DXX_LOCAL defined"
        )
    endif() # DXX_NO_CPM AND NOT DXX_LOCAL

    find_package( Doxygen REQUIRED )

    foreach( DXX_MODULE IN LISTS DXX_MODULES )
        if( DXX_LOCAL )
            # Get from local storage
            list( APPEND DXX_DOC_DIRS "${DXX_LOCAL}/${DXX_MODULE}/src" )
        else()
            # Get from CPM
            list( APPEND DXX_DOC_DIRS "${dot-xx-${DXX_MODULE}_SOURCE_DIR}/src" )
        endif()
    endforeach() # DXX_MODULE IN LISTS DXX_MODULES

    message( STATUS "Doxygen source paths: ${DXX_DOC_DIRS}" )

    set( DOXYGEN_EXTENSION_MAPPING ".xx=C++" )
    set( DOXYGEN_FILE_PATTERNS "*.xx" )
    doxygen_add_docs( docs ${DXX_DOC_DIRS} )
endif()
