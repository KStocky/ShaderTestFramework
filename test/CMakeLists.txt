include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/ExternalProjects.cmake")

add_executable(ShaderTestFrameworkTests)

set(SOURCES
    Container/RingBufferTests.cpp
    D3D12/DescriptorFreeListAllocatorTests.cpp
    D3D12/DescriptorRingAllocatorTests.cpp
    D3D12/DescriptorTests.cpp
    D3D12/Shader/ShaderCompilerTests.cpp
    D3D12/Shader/VirtualShaderDirectoryMappingManagerTests.cpp
    Framework/ShaderTestFixtureTests.cpp
    Utility/ConceptsTests.cpp
    Utility/EnumReflectionTests.cpp
    Utility/FunctionTraitsTests.cpp
    Utility/LambdaTests.cpp
    Utility/TypeListTests.cpp
    Utility/TypeTraitsTests.cpp
)

target_sources(ShaderTestFrameworkTests PRIVATE ${SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})

copy_all_dependent_dlls(ShaderTestFrameworkTests)
add_catch2(ShaderTestFrameworkTests)