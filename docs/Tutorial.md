<a id="top"></a>
# Tutorial

**Contents**<br>
[Requirements](#requirements)<br>
[Optional Requirements](#optional-requirements)<br>
[Getting ShaderTestFramework](#getting-shader-test-framework)<br>

## Requirements

1. Windows 10 Version 1909 (OS build 18363.1350) or greater - This is due to a dependency on the [DirectX Agility SDK](https://devblogs.microsoft.com/directx/gettingstarted-dx12agility/)
2. Visual Studio 2022 17.2 - This is due to the project making use of several C++23 features e.g. [Deducing This](https://devblogs.microsoft.com/cppblog/cpp23-deducing-this/)
3. [CMake 3.25](https://cmake.org/download/) - Shader Test Framework's cmake scripts make use of [PROPAGATE](https://cmake.org/cmake/help/latest/command/return.html) which came in 3.25 
4. A working internet connection on first build - This is due to the cmake scripts downloading all of the dependencies of the project.

## Optional Requirements

1. [PIX on Windows](https://devblogs.microsoft.com/pix/download/) - This is not required. However, it is recommended. The framework is able to take pix captures of your test runs. This will allow you to be able to debug your shader code. Very useful for tracking down the cause of a failing test.
2. A testing framework - [Catch2](https://github.com/catchorg/Catch2) is the recommended framework and all examples will use it. However, most other frameworks should be ok. NOTE: [MS Unit Testing Framework](https://learn.microsoft.com/en-us/visualstudio/test/writing-unit-tests-for-c-cpp?view=vs-2022) is not likely to work. Shader Test Framework depends on being able to copy dlls (e.g the DirectX Agility SDK dlls) to the location of the executable being run. Microsofts testing framework does not produce an exe. It produces a dll which is picked up by the testing framework's exe which is located in the installation of Visual Studio. 

## Getting Shader Test Framework

Ideally you should be using Shader Test Framework through its [CMake integration](cmake-integration.md#top).
Otherwise following these steps should work:

1. Grab the repo through any method
2. Create a new directory in the root of the project and name it something like `build`
3. Open a new command prompt in this directory
4. run `cmake ..` - This will generate the visual studio solution for Shader Test Framework. At this point if you do not wish to continue using CMake you do not have to. You can just open the Visual Studio solution and work as normal from there. However, CMake is recommended.
5. run `cmake --build .` - This will build everything and produce a static library called ShaderTestFramework in `ShaderTestFramework\build\src\ShaderTestFramework\Debug`. If you want a release build run `cmake --build . -C Release`
6. Link your project with the produced lib

But again. CMake is recommended.

## Writing tests

[Home](Readme.md#top)
