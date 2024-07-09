cmake_minimum_required(VERSION 3.26)

project(
    ShaderTestingFramework 
    VERSION 0.1.0 
    DESCRIPTION "A testing framework for shader code"
    LANGUAGES CXX )

if(CMAKE_GENERATOR MATCHES "Visual Studio" AND CMAKE_VS_VERSION_BUILD_NUMBER VERSION_LESS "17.7")
    message(FATAL_ERROR "Shader Test Framework requires at least Visual Studio 17.7. You have ${CMAKE_VS_VERSION_BUILD_NUMBER} installed. Please update your installation of VS 2022 to latest")
endif()

if(NOT MSVC)
    message(FATAL_ERROR "Shader Test Framework requires the MSVC compiler. If you are using Ninja as the generator, set cl as the value of CMAKE_CXX_COMPILER")
endif()

if(MSVC_VERSION VERSION_LESS 1937)
    message(FATAL_ERROR "Shader Test Framework requires at least MSVC 1937 (Visual Studio 2022 17.7 ). You have ${MSVC_VERSION} installed. Please update your installation of VS 2022 to latest or if you have a later version installed point CMake to it using the toolset configuration preset.")
endif()

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

option(STF_ENABLE_TESTING "Enable Test Project")
option(STF_INCLUDE_EXAMPLES "Include and build examples")
option(STF_DISABLE_UNITY_STF "Disable Unity build for STF Targets")
option(STF_STRICT_COMPILATION "Turn Warning Level Up and Warnings as Errors")

if (${STF_STRICT_COMPILATION})
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

    set(GLOBAL_COMPILE_FLAGS 
        $<$<CXX_COMPILER_ID:MSVC>:/W4> 
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic> )

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
