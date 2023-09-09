cmake_minimum_required(VERSION 3.26)
project(
    ShaderTestingFramework 
    VERSION 0.1.0 
    DESCRIPTION "A testing framework for shader code"
    LANGUAGES CXX )

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()
add_subdirectory(test)

