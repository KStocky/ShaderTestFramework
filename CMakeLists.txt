cmake_minimum_required(VERSION 3.26)

message(STATUS "CMAKE version: ${CMAKE_VERSION}")

project(
    ShaderTestingFramework 
    VERSION 0.1.0 
    DESCRIPTION "A testing framework for shader code"
    LANGUAGES CXX )

if(MSVC_VERSION VERSION_LESS 1944)
    message(FATAL_ERROR "Shader Test Framework requires at least MSVC 1944 (Visual Studio 2022 17.14 ). You have ${MSVC_VERSION} installed. Please update your installation of VS 2022 to latest or if you have a later version installed point CMake to it using the toolset configuration preset.")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19)
    message(FATAL_ERROR "Requires clang-cl 19 or newer")
endif()

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

option(STF_ENABLE_TESTING "Enable Test Project")
option(STF_INCLUDE_EXAMPLES "Include and build examples")
option(STF_DISABLE_UNITY_STF "Disable Unity build for STF Targets")
option(STF_STRICT_COMPILATION "Turn Warning Level Up and Warnings as Errors")

include(Configuration)

addDynamicDebuggingConfig()

if (${STF_STRICT_COMPILATION})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

    set(GLOBAL_COMPILE_FLAGS /W4)

    list(APPEND GLOBAL_COMPILE_FLAGS
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
        # We need to disable this because of a known issue in Clang https://github.com/llvm/llvm-project/issues/51619
        # This warning basically makes it impossible to use tuplet as it is implemented as an aggregate.
        -Wno-missing-braces
        
        ## We need to disable this because of an issue in PixEvents.
        ## We can remove this when https://github.com/microsoft/PixEvents/pull/11 is resolved
        -Wno-unused-but-set-variable
        >)

endif()

add_subdirectory(src)

if (${STF_ENABLE_TESTING} OR ${STF_INCLUDE_EXAMPLES})
    enable_testing()

    if(${STF_ENABLE_TESTING})
        add_subdirectory(test)
    endif()

    if(${STF_INCLUDE_EXAMPLES})
        add_subdirectory(examples)
    endif()
endif()
