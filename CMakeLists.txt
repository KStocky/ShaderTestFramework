cmake_minimum_required(VERSION 3.26)
project(
    ShaderTestingFramework 
    VERSION 0.1.0 
    DESCRIPTION "A testing framework for shader code"
    LANGUAGES CXX )

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

set(GLOBAL_COMPILE_FLAGS 
    $<$<CXX_COMPILER_ID:MSVC>:/W4> 
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> )

find_package(D3D12AgilitySdk REQUIRED)
find_package(WARPPackage REQUIRED)
find_package(DXC REQUIRED)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

