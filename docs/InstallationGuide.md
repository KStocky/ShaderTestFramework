[Reference](./ShaderTestFramework.md)

# Installation Guide

**Contents**<br>
[CMake Installation](#cmake-installation)<br>
[Installing from git repository](#installing-shader-test-framework-from-git-repository)<br>

Because we use CMake to build Shader Test Framework, we also provide a couple of
integration points for our users.

1) Shader Test Framework exports a CMake target
2) Shader Test Framework's repository contains CMake scripts for adding Catch2 as a testing framework and for managing assets

## CMake Installation

CMake is the recommended way of using Shader Test Framework. Shader Test Framework's CMake build exports `ShaderTestFramework`. Linking against
it will add the proper include paths and link your target together with Shader Test Framework.

Installing via [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) is recommended. It will grab STF and all of it's dependencies
The following is an example of how this is done:

```cmake
Include(FetchContent)

FetchContent_Declare(
  stf
  GIT_REPOSITORY https://github.com/KStocky/ShaderTestFramework
  GIT_TAG main
)

FetchContent_MakeAvailable(stf)
list(APPEND CMAKE_MODULE_PATH ${stf_SOURCE_DIR}/cmake)
include(AssetDependencyManagement)
include(STF)

add_executable(MyTestTarget test.cpp)
asset_dependency_init(MyTestTarget)
add_stf_with_catch2(MyTestTarget)
```

## Installing Shader Test Framework from git repository

Assuming you have enough rights, you can just
install it to the default location, like so:
```
$ git clone https://github.com/KStocky/ShaderTestFramework
$ cd ShaderTestFramework
$ cmake --workflow --preset FullDev
```

This will build everything, including tests and examples. It will produce a static library called ShaderTestFramework in `ShaderTestFramework\build\src\ShaderTestFramework\Debug`. If you want a release build run `cmake --build --preset FullDevRelease`.
You can also generate a project that just contains the library and tests (omitting examples) with `cmake --workflow --preset SlimDev`. More [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) that are defined by STF can be found in [CMakePresets.json](../CMakePresets.json)
And feel free to create your own local `CMakeUserPresets.json` to improve your workflow.

From here you can link ShaderTestFramework to your project and add `ShaderTestFramework\src\ShaderTestFramework\Public` to your include paths.

