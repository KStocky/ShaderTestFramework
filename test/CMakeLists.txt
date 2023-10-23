include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/ExternalProjects.cmake")

add_executable(ShaderTestFrameworkTests)
target_sources(ShaderTestFrameworkTests PRIVATE 
    BasicTests.cpp
    ConceptsTests.cpp
    DescriptorFreeListAllocatorTests.cpp
    DescriptorRingAllocatorTests.cpp
    DescriptorTests.cpp
    FunctionTraitsTests.cpp
    RingBufferTests.cpp
    ShaderCompilerTests.cpp
    ShaderTestFixtureTests.cpp
    TypeListTests.cpp
    TypeTraitsTests.cpp
    )

target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})

copy_all_dependent_dlls(ShaderTestFrameworkTests)
add_catch2(ShaderTestFrameworkTests)