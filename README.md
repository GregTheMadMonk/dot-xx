# `.xx`

My personal collection of small C++ modules put in this repo as convenient to
share between projects.

All modules require C++23's `import std`. This means at least __CMake 3.30__ and
__Ninja__ (what version?)

|Module|C++ version|Depends on|Description|
|---|---|---|---|
|[assert](https://github.com/GregTheMadMonk/dot-xx/tree/assert)| C++23 | - | Provides `dxx::assert::always`/`dxx::assert::debug` functions. Makes use of `<stacktrace>` if available |
|[cstd](https://github.com/GregTheMadMonk/dot-xx/tree/cstd)| C++23 | - | Exports some of the things not provided by `std.compat` module (like `stdout`/`stdin`/etc.) |
|[selftest](https://github.com/GregTheMadMonk/dot-xx/tree/selftest)| C++23 | [assert](https://github.com/GregTheMadMonk/dot-xx/tree/assert), [cstd](https://github.com/GregTheMadMonk/dot-xx/tree/cstd) | Simple unit testing |

### Navigating the repo

Each module has its own dedicated branch where its source code is located.
The links at the top of this __README__ will take you to branches that contain
corrseponding modules. `src/` directory contains the module source code and
`test/` directory (if present) contains the unit tests (utilizing
[selftest](https://github.com/GregTheMadMonk/dot-xx/tree/selftest)) for the
module.

The reasnoning for such weird (and probably bad) decision is the following:
the modules are small enough that they don't deserve a separate repo and an
entire organization to hold these repos, but I still want to preserve the
ability to use them separately when possible.

### Using the repo

Each module branch has its own individual `CMakeLists.txt` file and could
be used (unless dependent on) without any other module. The `CMakeLists.txt`
for individual modules are very simple: they do not set any unnecessary flags
(like forcing `-stdlib=libc++`, turning `CMAKE_CXX_MODULE_STD` __ON__ or setting
`CMAKE_EXPERIMENTAL_CXX_IMPORT_STD`). Managing all this is left to the user.

This branch (`main`) contains no code other than a `CMakeLists.txt` that pulls
all modules available in this repo via [CPM](https://github.com/cpm-cmake/CPM.cmake)
and a basic `selftest` executable for library unit-testing.
Local builds without pulling from __GitHub__ are also supported, see below.

#### Using the whole repo via [CPM](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage( "gh:gregthemadmonk/dot-xx#main" )

target_link_libraries( your_target PRIVATE dot-xx::all )      # link all modules
target_link_libraries( your_target PRIVATE dot-xx::selftest ) # link only selftest and required
```

#### Using only a specific module with [CPM](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage(
    NAME "dot-xx-selftest"
    GITHUB_REPOSITORY "gregthemadmonk/dot-xx"
    GIT_TAG "selftest"
)
```

#### Using only a specific module without [CPM](https://github.com/cpm-cmake/CPM.cmake)

```cmake
set( DXX_NO_CPM ON )
add_subdirectory( path/to/dot-xx/assert   ) # selftest depends on asssert
add_subdirectory( path/to/dot-xx/cstd     ) # selftest depends on cstd
add_subdirectory( path/to/dot-xx/selftest ) # the module we need
```

#### Dependency management

Dependency pulling in `CMakeLists.txt` for individual modules and in `main`'s
`CMakeLists.txt` is done via [CPM](https://github.com/cpm-cmake/CPM.cmake).
You can opt out of this by setting `DXX_NO_CPM` variable in __CMake__.
In that case you must manually ensure that the modules are added to your project
in correct order (depenant modules after the ones they depend on).

If you're not using `main`'s `CMakeLists.txt` you must also note that
[CPM](https://github.com/cpm-cmake/CPM.cmake) is not included with individual
modules and must be provided by you.

#### Documentation

Building documentation is supported if `DXX_DOXYGEN` variable is set in
__CMake__. To generate documentation, run `ninja docs` in the build directory.

## Building

The command line I personally use:
```sh
mkdir build && cd build
CC=clang CXX=clang++ cmake .. -GNinja \
                              -DCMAKE_CXX_FLAGS='-stdlib=libc++' \
                              -DCMAKE_EXPERIMENTAL_CXX_IMPORT_STD='0e5b6991-d74f-4b3d-a41c-cf096e0b2508' \
                              -DDXX_SELFTEST=ON \
                              -DDXX_DOXYGEN=ON
```

Don't forget to initialize the __git submodules__ for
[CPM](https://github.com/cpm-cmake/CPM.cmake) to work like this.

### Building with local modules

Development workflow revolves around __git worktrees__. You are expected to have
a directory `/path/to/dir` with subdirectories named after all the modules and
containing them. You then pass `-DDXX_LOCAL=/path/to/dir` to __CMake__,
which will make it use your local copies of modules instead of fetching them
from __GitHub__.

### Contributing etc.

There are no guidelines yet. Open issues and PRs as you wish.
