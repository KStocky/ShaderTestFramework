include("${CMAKE_HOME_DIRECTORY}/cmake/AssetDependencyManagement.cmake")
include("${CMAKE_HOME_DIRECTORY}/cmake/ExternalProjects.cmake")

add_executable(ShaderTestFrameworkTests)

set(SHADER_SOURCES
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ArrayTemplateSpecialization.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/CleanupAttribute.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/CounterMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DeferSFINAE.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DefineStructMemberFunctionInFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DefineStructMemberFunctionLater.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ForwardDeclareFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ForwardDeclareFunctionInOtherFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ForwardDeclareFunctionInOtherFunctionAndDefineIt.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/FunctionStyleMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/GlobalString.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/HasMemberCheck.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/LineMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/LocalString.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/LocalStringAssignedToAUintArray.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/MacroGeneratedStructWithStaticDataMember.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/NestedStructTemplates.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorBool.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorBoolWithAnEmptyStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorInt.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorIntWithAnEmptyStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/SizeofOperator.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticConstStructMemberInTemplatedStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticStructMemberInNonTemplatedStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticStructMemberInTemplatedStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticVariableInTemplatedFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/TernaryInTypeTrait.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/VariadicMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ResourceBinding/ArrayOfBuffers.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ResourceBinding/SingleInputBuffer.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ResourceBinding/UnboundedArrayOfBuffers.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderIncludeHandlerTests/Include.hlsli
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderIncludeHandlerTests/ShaderWithInclude.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ClosestHitShaderWithAttributes.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ClosestHitShaderWithNoAttributes.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ComputeShaderWith16BitFloats.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ComputeShaderWith64BitIntegers.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ComputeShaderWithDynamicResources.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ComputeShaderWithSinglePrecisionDotAndAccumulate.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/ComputeShaderWithWritableMSAATextures.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/PixelShaderWithQuadAny.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/PixelShaderWithSamplerFeedback.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/PixelShaderWithSV_Barycentrics.hlsl
    Shader/Framework/ShaderTestFixtureTests/BasicShaderTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/AssertBufferSizeTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/AreEqual.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/Fail.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/IsFalse.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/IsTrue.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Asserts/NotEqual.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Bugs/GIVEN_Object_WHEN_ConversionOperatorCalled_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Bugs/GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ByteWriter/ByteWriterTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Cast/GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Cast/GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Cast/GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_LoadCalled_THEN_ReturnsExpectedValue.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_StoreCalled_THEN_Succeeds.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_StoreCalledWithDifferentType_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store2CalledWithDifferentType_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store3CalledWithDifferentType_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store4Called_THEN_Succeeds.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store4CalledWithDifferentType_THEN_Fails.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/FlattenIndex.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/NumArgs.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/Scenario.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/Sections.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/Macros/SectionVarCreation.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ProofOfConcept/GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ProofOfConcept/GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ProofOfConcept/GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ProofOfConcept/GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ProofOfConcept/SectionTest.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ProofOfConcept/VariadicMacroOverloading.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/SectionManagement.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ThreadDimensionTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/ThreadIdRegistrationTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/TypeTraits/ArrayTraitsTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/TypeTraits/ContainerTraitsTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/TypeTraits/FundamentalTypeTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/TypeTraits/FundamentalTypeTraitsTests.hlsl
    Shader/Framework/ShaderTestFixtureTests/HLSLFrameworkTests/TypeTraits/TypeIDTests.hlsl
)

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

set(SHADER_SOURCE_REL_DIR "ShaderTests")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shader)

target_sources(ShaderTestFrameworkTests PRIVATE ${SHADER_SOURCES})
target_sources(ShaderTestFrameworkTests PRIVATE ${SOURCES})

set_source_files_properties(${SHADER_SOURCES} PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")
set_target_properties(ShaderTestFrameworkTests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ShaderTestFrameworkTests>" )

source_group(TREE ${SHADER_SOURCE_DIR} PREFIX "ShaderSources" FILES ${SHADER_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_compile_definitions(ShaderTestFrameworkTests PRIVATE SHADER_SRC="${SHADER_SOURCE_REL_DIR}" USE_PIX)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})
target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework)

target_add_asset_directory(ShaderTestFrameworkTests ${SHADER_SOURCE_DIR} "/${SHADER_SOURCE_REL_DIR}")
copy_all_dependent_assets(ShaderTestFrameworkTests)

add_catch2(ShaderTestFrameworkTests)
