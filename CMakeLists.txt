cmake_minimum_required(VERSION 3.26)
include("${CMAKE_HOME_DIRECTORY}/cmake/AssetDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/Nuget.cmake")

project(
    ShaderTestingFramework 
    VERSION 0.1.0 
    DESCRIPTION "A testing framework for shader code"
    LANGUAGES CXX )

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(GLOBAL_COMPILE_FLAGS 
    $<$<CXX_COMPILER_ID:MSVC>:/W4> 
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> )

dll_dependency_init()
nuget_initialize("${CMAKE_HOME_DIRECTORY}/extern")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

