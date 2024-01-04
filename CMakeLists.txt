cmake_minimum_required(VERSION 3.26)

project(
    ShaderTestingFramework 
    VERSION 0.1.0 
    DESCRIPTION "A testing framework for shader code"
    LANGUAGES CXX )

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(AssetDependencyManagement)
include(Nuget)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

set(DEV_MODE CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

if (${DEV_MODE})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

    set(GLOBAL_COMPILE_FLAGS 
        $<$<CXX_COMPILER_ID:MSVC>:/W4> 
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> )

endif()

add_subdirectory(src)

if (${DEV_MODE})
    enable_testing()
    add_subdirectory(examples)
    add_subdirectory(test)
endif()

