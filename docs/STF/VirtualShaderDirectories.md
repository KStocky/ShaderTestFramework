[Reference](../ShaderTestFramework.md)

# Virtual Shader Directories

**Contents**<br>
1. [An Introduction to Virtual Shader Directories](#an-introduction-to-virtual-shader-directories)<br>
2. [Dealing with Shader Directories using CMake](#dealing-with-shader-directories-using-cmake)<br>
    a. [Asset Dependency Management Library](#asset-dependency-management-library)<br>
    b. [The Working Directory](#the-working-directory)<br>
    c. [Passing Directory Mappings from CMake to C++](#passing-directory-mappings-from-cmake-to-c)<br>
3. [Setting up Virtual Shader Directory Mappings in C++](#setting-up-virtual-shader-directory-mappings-in-c)<br>
4. [Using Virtual Shader Directories in HLSL](#using-virtual-shader-directories-in-hlsl)<br>

## An Introduction to Virtual Shader Directories

It can be useful to have virtual mappings to directories for asset like files. This decouples the actual location of the asset files themselves on disk, from the directory structure of the assets. A good example of the use of Virtual Paths is [Unreal Engine](https://dev.epicgames.com/documentation/en-us/unreal-engine/overview-of-shaders-in-plugins-unreal-engine). In Unreal Engine, a game's Content folder will be mapped to the virtual path, "/Game". Similarly, plugins can provide their own virtual path mappings for shaders of that plugin.

Shader Test Framework provides a very similar system for handling a test suite's shader files. An example project of this has been provided with ([Ex2_VirtualShaderDirectories](../../examples/Ex2_VirtualShaderPaths))

## Dealing with Shader Directories using CMake

The following sections will be refering to ([Ex2_VirtualShaderDirectories](../../examples/Ex2_VirtualShaderPaths/CMakeLists.txt)). This CMakeLists.txt has comments which document what each line is doing but the following sections will be going into more explanation of them.

### Asset Dependency Management Library
Shader files are essentially asset files. They are files that do not take part in the build of the final executable, but are still part of the project. Therefore, for STF to use them, STF must be able to find them on disk and load them. To give test writers the flexiblity of defining where a test suite's shaders are, STF provides a CMake library, called AssetDependencyManagement, that aids in the handling of asset files. This library has 3 main functions:

1) asset_dependency_init - This function will initialize the library for a specified target. This function takes a single parameter which is the name of the target that you will be using this library for.
2) target_add_asset_directory - This function will set up an asset mapping for a particular directory. This functions 3 parameters. The first is the target that this asset mapping is to be applied to. The second is the ABSOLUTE path of the directory that is to be mapped. The third is the RELATIVE path from the target's executable that the source directory will be mapped to.
3) copy_all_dependent_assets - This function will add a post build step to your target which will copy all directories mapped using target_add_asset_directory for your target AND all targets that this target depends upon. This ensures that a target doesn't need to worry about any dependencies that may also need to have asset files copied around. This is all handled automatically. This function takes a single parameter which is the name of the target.

Below are the relevant lines from ([Ex2_VirtualShaderDirectories](../../examples/Ex2_VirtualShaderPaths/CMakeLists.txt)). 

```cmake
asset_dependency_init(Ex2_VirtualShaderPaths)
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ShaderCode)
set(SHADER_DEST_DIR "./ShaderCode")
target_add_asset_directory(Ex2_VirtualShaderPaths ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR})
target_link_libraries(Ex2_VirtualShaderPaths PRIVATE ShaderTestFramework)
copy_all_dependent_assets(Ex2_VirtualShaderPaths)
```

1) `asset_dependency_init(Ex2_VirtualShaderPaths)` - We initialize the asset dependency library for our project
2) `set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ShaderCode)` - We store the absolute path of our shader code in a variable called SHADER_SOURCE_DIR
3) `set(SHADER_DEST_DIR "./ShaderCode")` - We store the path that we want our shader code to be copied to after the build. This path is relative to the executable
4) `target_add_asset_directory(Ex2_VirtualShaderPaths ${SHADER_SOURCE_DIR} ${SHADER_DEST_DIR})` - We define our mapping. The contents of the directory `SHADER_SOURCE_DIR` will be copied to the relative path `SHADER_DEST_DIR` after the build.
5) `target_link_libraries(Ex2_VirtualShaderPaths PRIVATE ShaderTestFramework)` - We link our target with the ShaderTestFramework.
6) `copy_all_dependent_assets(Ex2_VirtualShaderPaths)` - This sets up the post build steps to copy each asset directory specified for this target and all dependent targets. It is important that this is called AFTER all libraries have been linked with `target_link_libraries` and all asset directory dependencies have been specified with `target_add_asset_directory`.


### The Working Directory

When a program is executed, it will be assigned a "working directory". This is the path that is used to resolve relative paths for the process. This is typically (but not always) the directory of the executable itself. A good example of when the working directory is NOT the directory of the executable is when running your project through Visual Studio. This often trip people up when they start working with files with a visual studio project for the first time. Loading a file will work when running through Visual Studio, but if you run the executable from Windows Explorer, it will fail to find any of the files it tries to load. To prevent any confusion such as this, it is possible to ensure that Visual Studio does not do this from CMake. Below is the relevant line from ([Ex2_VirtualShaderDirectories](../../examples/Ex2_VirtualShaderPaths/CMakeLists.txt)).

`set_target_properties(Ex2_VirtualShaderPaths PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Ex2_VirtualShaderPaths>" )`

This will set the `VS_DEBUGGER_WORKING_DIRECTORY` property of `Ex2_VirtualShaderPaths` to be the directory of the final executable. Just like it would be if we just ran the executable without Visual Studio. So with this command, we ensure that when running the project from Visual Studio we are getting the same behaviour as if we were running it from anywhere else.

### Passing Directory Mappings from CMake to C++

Since we are defining where shader files are going to be placed post build, it can be useful to pass that information on to our C++ code from CMake. This ensures that there is a single source of truth for this information which makes changing directories in the future much easier.

To do that, we can pass this information through as a define to our C++ code. Below is the relevant line from ([Ex2_VirtualShaderDirectories](../../examples/Ex2_VirtualShaderPaths/CMakeLists.txt))

`target_compile_definitions(Ex2_VirtualShaderPaths PRIVATE SHADER_SRC="${SHADER_DEST_DIR}")`

This will set a private define (so any other target that depends on this one will not get this define) called `SHADER_SRC` to the relative path of the shader directory.

## Setting up Virtual Shader Directory Mappings in C++

The following section will be referring to ([VirtualShaderPaths.cpp](../../examples/Ex2_VirtualShaderPaths/VirtualShaderPaths.cpp)). Just like the CMakeLists.txt that we went through [above](#dealing-with-shader-directories-using-cmake), this C++ file is commented to explain what is going on. However, this document will be going into more detail.

When creating our `ShaderTestFixture` for a shader test suite we are able to create virtual mappings of directories that both the Shader Compiler and the fixture itself will use. To create a mapping we simply append to the `ShaderTestFixture::Desc::Mappings` member. 

Mappings is a `std::vector<VirtualShaderDirectoryMapping>`. `VirtualShaderDirectoryMapping` is defined as follows:

```c++
struct VirtualShaderDirectoryMapping
{
	std::filesystem::path VirtualPath;
	std::filesystem::path RealPath;
};
```

1) `VirtualPath` - A virtual path is deemed as any path that is prefixed with a '/'.
2) `RealPath` - This is the ABSOLUTE path to the real directory that is represented by the `VirtualPath`

If a path that either the `ShaderTestFixture` or the shader compiler comes across starts with a known virtual path, that virtual path will be replaced with the associated `RealPath`.

Below is the relevant line from ([VirtualShaderPaths.cpp](../../examples/Ex2_VirtualShaderPaths/VirtualShaderPaths.cpp)) that creates a virtual mapping

`desc.Mappings.emplace_back(VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC});`

[`std::filesystem::current_path()`](https://en.cppreference.com/w/cpp/filesystem/current_path) returns the current Working Directory path of the process. `SHADER_SRC` is the define that we created in [Passing Directory Mappings from CMake to C++](#passing-directory-mappings-from-cmake-to-c). Therefore this mapping will map the shader directory that we created in [Asset Dependency Management Library](#asset-dependency-management-library) to the virtual directory `"/Shader"`.

This mapping is then used in the following line:

`desc.Source = std::filesystem::path{ "/Shader/MyShaderTests.hlsl" };`

Here we are telling the fixture that our shader that we want to compile is in the directory that is represented by the virtual directory `"/Shader"`

## Using Virtual Shader Directories in HLSL

The `ShaderTestFixture` compiles its HLSL code using a custom `IDxcIncludeHandler` which simply does the virtual mapping substitution so that shader `#include`s can also make use of the virtual directories. An example of this can be found in [MyShaderTests.hlsl](../../examples/Ex2_VirtualShaderPaths/ShaderCode/MyShaderTests.hlsl):

`#include "/Shader/MyCoolHLSLFunction.hlsli"`

---

[Top](#virtual-shader-directories)

