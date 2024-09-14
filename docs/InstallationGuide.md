[Reference](./ShaderTestFramework.md)

# Installation Guide

**Contents**<br>
1. [CMake Installation](#cmake-installation)<br>
2. [Installing from git repository](#installing-shader-test-framework-from-git-repository)<br>
3. [Building with Ninja](#building-with-ninja)<br>
a. [Building with Ninja from the Command Line](#building-with-ninja-from-the-command-line)<br>

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

[ShaderTestFrameworkExamples](https://github.com/KStocky/ShaderTestFrameworkExamples) is a repository that demonstrates how to set up a cmake project that makes use of `ShaderTestFramework`. It also has a github workflow to demonstrate how to run your tests on github actions.

## Installing Shader Test Framework from git repository

Assuming you have enough permissions, you can download and build it like so:
```
$ git clone https://github.com/KStocky/ShaderTestFramework
$ cd ShaderTestFramework
$ cmake --workflow --preset VS2022Build
```

This will create a Visual Studio 2022 solution, and then build everything, including tests and examples. It will produce a static library called ShaderTestFramework in `ShaderTestFramework\build\src\ShaderTestFramework\Debug`. If you want a release build you could create a new build preset for a release build. STF's [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) can be found in [CMakePresets.json](../CMakePresets.json)
And feel free to create your own local `CMakeUserPresets.json` to improve your workflow.

From here you can link ShaderTestFramework to your project and add `ShaderTestFramework\src\ShaderTestFramework\Public` to your include paths.

## Building with Ninja

In the previous section we mentioned that you can use `$ cmake --workflow --preset VS2022Build` to create a Visual Studion 2022 solution to build the project. However, STF can also be built using Ninja. The main benefit to using Ninja as opposed to Visual Studio 2022 to bulid is speed. Full project compilations are 10x faster with Ninja in comparison to VS2022. There are two ways to build with Ninja rather than with a VS sln.

1. You can make use of Visual Studio's CMake integration by simply opening the root folder in Visual Studio 2022. [CMake Projects in Visual Studio](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170)

2. Run CMake commands from an x64 Native Tools command line. This might be preferred if you would rather not have to use Visual Studio.

### Building with Ninja from the Command Line

It is not enough to just run `cmake --workflow --preset NinjaBuild` from the command line unfortunately. Prior to running this command, you must set up the developer environment in your command prompt. To do this open the `x64 Native Tools Command Prompt for VS 2022`. A simple search in the windows search bar should find it. Then navigate `cd` to the project root. At this point running `cmake --workflow --preset NinjaBuild` will create a Ninja project and build.

