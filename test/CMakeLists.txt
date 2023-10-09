include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
find_package(Catch2 REQUIRED)

add_executable(ShaderTestFrameworkTests)
target_sources(ShaderTestFrameworkTests PRIVATE 
    BasicTests.cpp
    DescriptorFreeListAllocatorTests.cpp
    DescriptorRingAllocatorTests.cpp
    DescriptorTests.cpp
    RingBufferTests.cpp
    ShaderCompilerTests.cpp
    ShaderTestFixtureTests.cpp)

target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework Catch2::Catch2WithMain)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})

copy_all_dependent_dlls(ShaderTestFrameworkTests)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)

catch_discover_tests(ShaderTestFrameworkTests)
