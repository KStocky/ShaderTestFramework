include("${CMAKE_HOME_DIRECTORY}/cmake/DllDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/ExternalProjects.cmake")

add_executable(ShaderTestFrameworkTests)

set(SHADER_SOURCES
    Shader/Framework/ShaderTestFixtureTests/BasicShaderTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/AreEqual.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/Fail.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/IsFalse.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/IsTrue.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/NotEqual.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Bugs/GIVEN_Object_WHEN_ConversionOperatorCalled_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Bugs/GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Cast/GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Cast/GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Cast/GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/Scenario.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/Sections.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/SectionVarCreation.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/FlattenIndex.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/SectionManagement.hlsl
    Shader/Framework/ShaderTestFixtureTests/ShaderFrameworkHLSLProofOfConceptTests.hlsl
)

set_source_files_properties(${SHADER_SOURCES} PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")
target_sources(ShaderTestFrameworkTests PRIVATE ${SHADER_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/Shader PREFIX "ShaderSources" FILES ${SHADER_SOURCES})
set(SHADER_SOURCE_REL_DIR "ShaderTests")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shader)
target_compile_definitions(ShaderTestFrameworkTests PRIVATE SHADER_SRC="${SHADER_SOURCE_REL_DIR}" USE_PIX)
target_add_dll_directory(ShaderTestFrameworkTests ${SHADER_SOURCE_DIR} "/${SHADER_SOURCE_REL_DIR}" SRC_DIRS DEST_DIRS)



set(SOURCES
    Container/RingBufferTests.cpp
    D3D12/CommandEngineTests.cpp
    D3D12/CommandListTests.cpp
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
set_target_properties(ShaderTestFrameworkTests PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ShaderTestFrameworkTests>" )

copy_all_dependent_dlls(ShaderTestFrameworkTests)
add_catch2(ShaderTestFrameworkTests)