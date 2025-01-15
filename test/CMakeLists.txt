include(AssetDependencyManagement)
include(ExternalProjects)

add_executable(ShaderTestFrameworkTests)
asset_dependency_init(ShaderTestFrameworkTests)

set(SHADER_SOURCES
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/Alignof.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ArrayTemplateSpecialization.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/CallOperator.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/CleanupAttribute.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/CounterMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DeducingPackingOrder.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DeferSFINAE.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DefineStructMemberFunctionInFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/DefineStructMemberFunctionLater.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ExpressionSFINAE.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ForwardDeclareFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ForwardDeclareFunctionInOtherFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ForwardDeclareFunctionInOtherFunctionAndDefineIt.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/FunctionStyleMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/GloballyCoherentInStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/GloballyCoherentTypeModifier.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/GlobalString.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/HasMemberCheck.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/ImmediatelyInvokableFunctionExpression.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/IsEnum.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/LineMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/LocalString.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/LocalStringAssignedToAUintArray.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/MacroGeneratedStructWithStaticDataMember.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/NestedStructTemplates.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorBool.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorBoolWithAnEmptyStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorInt.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/OperatorIntWithAnEmptyStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/PrinterWithOptimizations.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/PrinterWithoutOptimizations.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/SizeofEnumType.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/SizeofOperator.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticAssert.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticConstStructMemberInTemplatedStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticStructMemberInNonTemplatedStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticStructMemberInTemplatedStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/StaticVariableInTemplatedFunction.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/TernaryInTypeTrait.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/UserDefinedVector.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/HLSLTests/VariadicMacro.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ReflectionTests/ArrayOfBuffers.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ReflectionTests/ManyTypesOfBindings.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ReflectionTests/SingleInputBuffer.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ReflectionTests/UnboundedArrayOfBuffers.hlsl
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
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderModelTests/TemplatedByteAddressBuffer.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/ConstantBufferCanBeBoundToRootConstants/ArrayBufferStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/ConstantBufferCanBeBoundToRootConstants/NonArrayBufferStructContainsArray.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/ConstantBufferCanBeBoundToRootConstants/NonArrayBufferStructContainsMemberThatContainsArray.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/ConstantBufferCanBeBoundToRootConstants/NonArrayBufferWithNoArrayMembers.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/ConstantBufferCanBeBoundToRootConstants/StructIsOver64Uints.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/IsOrContainsArray/ArrayBufferStruct.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/IsOrContainsArray/NonArrayBufferStructContainsArray.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/IsOrContainsArray/NonArrayBufferStructContainsMemberThatContainsArray.hlsl
    Shader/D3D12/Shader/ShaderCompilerTests/ShaderReflectionUtilsTests/IsOrContainsArray/NonArrayBufferWithNoArrayMembers.hlsl
    Shader/HLSLFrameworkTests/Asserts/AreEqual.hlsl
    Shader/HLSLFrameworkTests/Asserts/Fail.hlsl
    Shader/HLSLFrameworkTests/Asserts/IsFalse.hlsl
    Shader/HLSLFrameworkTests/Asserts/IsTrue.hlsl
    Shader/HLSLFrameworkTests/Asserts/NotEqual.hlsl
    Shader/HLSLFrameworkTests/Binding/ValueBindingsTests/GlobalBindings.hlsl
    Shader/HLSLFrameworkTests/Binding/ValueBindingsTests/GlobalBindingsTooLarge.hlsl
    Shader/HLSLFrameworkTests/Binding/ValueBindingsTests/MultipleConstantBuffers.hlsl
    Shader/HLSLFrameworkTests/Binding/ValueBindingsTests/WithArray.hlsl
    Shader/HLSLFrameworkTests/Bugs/ConversionOperator.hlsl
    Shader/HLSLFrameworkTests/ByteReaderTraits/ByteReaderTraitsTests.hlsl
    Shader/HLSLFrameworkTests/ByteWriter/ByteWriterTests.hlsl
    Shader/HLSLFrameworkTests/Cast/GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails.hlsl
    Shader/HLSLFrameworkTests/Cast/GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds.hlsl
    Shader/HLSLFrameworkTests/Cast/GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_LoadCalled_THEN_ReturnsExpectedValue.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_StoreCalled_THEN_Succeeds.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_StoreCalledWithDifferentType_THEN_Fails.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store2CalledWithDifferentType_THEN_Fails.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store3CalledWithDifferentType_THEN_Fails.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store4Called_THEN_Succeeds.hlsl
    Shader/HLSLFrameworkTests/Container/ArrayTests/GIVEN_IntArray_WHEN_Store4CalledWithDifferentType_THEN_Fails.hlsl
    Shader/HLSLFrameworkTests/Macros/AssertMacro.hlsl
    Shader/HLSLFrameworkTests/Macros/NumArgs.hlsl
    Shader/HLSLFrameworkTests/Macros/Scenario.hlsl
    Shader/HLSLFrameworkTests/Macros/Sections.hlsl
    Shader/HLSLFrameworkTests/Macros/SectionVarCreation.hlsl
    Shader/HLSLFrameworkTests/Macros/Stamp.hlsl
    Shader/HLSLFrameworkTests/Memory/AlignedOffsetTests.hlsl
    Shader/HLSLFrameworkTests/Memory/ZeroTests.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/MatesPrinter.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/SectionTest.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/StringsAndTemplates.hlsl
    Shader/HLSLFrameworkTests/ProofOfConcept/VariadicMacroOverloading.hlsl
    Shader/HLSLFrameworkTests/String/StringByteWriterTests.hlsl
    Shader/HLSLFrameworkTests/String/StringDefineTests.hlsl
    Shader/HLSLFrameworkTests/String/StringTests.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/ResultsProcessing/AssertInfoWithData.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/ResultsProcessing/AssertInfoWithNoData.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/ResultsProcessing/ByteReader.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/ResultsProcessing/ByteReader.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/ResultsProcessing/NoAssertBuffer.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/ResultsProcessing/SectionsWithStrings.hlsl
    Shader/HLSLFrameworkTests/TestDataBuffer/SizeTests.hlsl
    Shader/HLSLFrameworkTests/BasicShaderTests.hlsl
    Shader/HLSLFrameworkTests/ConceptsTests.hlsl
    Shader/HLSLFrameworkTests/FlattenIndex.hlsl
    Shader/HLSLFrameworkTests/ModelsTests.hlsl
    Shader/HLSLFrameworkTests/SectionHierarchyByteWriterTests.hlsl
    Shader/HLSLFrameworkTests/SectionManagement.hlsl
    Shader/HLSLFrameworkTests/ThreadDimensionTests.hlsl
    Shader/HLSLFrameworkTests/ThreadIdRegistrationTests.hlsl
    Shader/HLSLFrameworkTests/TypeTraitsTests.hlsl
)

set(SOURCES
    Private/Container/RingBufferTests.cpp
    Private/D3D12/BindlessFreeListAllocatorTests.cpp
    Private/D3D12/CommandEngineTests.cpp
    Private/D3D12/CommandListTests.cpp
    Private/D3D12/CommandQueueTests.cpp
    Private/D3D12/DescriptorFreeListAllocatorTests.cpp
    Private/D3D12/DescriptorHeapTests.cpp
    Private/D3D12/DescriptorRingAllocatorTests.cpp
    Private/D3D12/DescriptorTests.cpp
    Private/D3D12/FencedResourcePoolTests.cpp
    Private/D3D12/Shader/HLSLTests.cpp
    Private/D3D12/Shader/ReflectionTests.cpp
    Private/D3D12/Shader/ShaderBindingTests.cpp
    Private/D3D12/Shader/ShaderHashTests.cpp
    Private/D3D12/Shader/ShaderIncludeHandlerTests.cpp
    Private/D3D12/Shader/ShaderModelTests.cpp
    Private/D3D12/Shader/ShaderReflectionUtilsTests.cpp
    Private/D3D12/Shader/VirtualShaderDirectoryMappingManagerTests.cpp
    Private/Framework/HLSLFramework/Asserts/AreEqualTests.cpp
    Private/Framework/HLSLFramework/Asserts/FailTests.cpp
    Private/Framework/HLSLFramework/Asserts/IsFalseTests.cpp
    Private/Framework/HLSLFramework/Asserts/IsTrueTests.cpp
    Private/Framework/HLSLFramework/Asserts/NotEqualTests.cpp
    Private/Framework/HLSLFramework/Binding/ValueBindingsTests.cpp
    Private/Framework/HLSLFramework/Container/ArrayTests.cpp
    Private/Framework/HLSLFramework/Macros/AssertMacroTests.cpp
    Private/Framework/HLSLFramework/Macros/NumArgsTests.cpp
    Private/Framework/HLSLFramework/Macros/ScenarioTests.cpp
    Private/Framework/HLSLFramework/Macros/SectionsTests.cpp
    Private/Framework/HLSLFramework/Macros/SectionVarCreationTests.cpp
    Private/Framework/HLSLFramework/Macros/StampTests.cpp
    Private/Framework/HLSLFramework/Memory/AlignedOffset.cpp
    Private/Framework/HLSLFramework/Memory/ZeroTests.cpp
    Private/Framework/HLSLFramework/String/StringByteWriterTests.cpp
    Private/Framework/HLSLFramework/String/StringDefineTests.cpp
    Private/Framework/HLSLFramework/String/StringTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/ResultsProcessing/AssertInfoWithDataTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/ResultsProcessing/AssertInfoWithNoDataTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/ResultsProcessing/ByteReaderTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/ResultsProcessing/FundamentalByteReaderTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/ResultsProcessing/NoAssertBufferTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/ResultsProcessing/SectionsWithStringsTests.cpp
    Private/Framework/HLSLFramework/TestDataBuffer/SizeTests.cpp
    Private/Framework/HLSLFramework/BasicShaderTests.cpp
    Private/Framework/HLSLFramework/Bugs.cpp
    Private/Framework/HLSLFramework/ByteReaderTraitsTests.cpp
    Private/Framework/HLSLFramework/ByteWriterTests.cpp
    Private/Framework/HLSLFramework/CastTests.cpp
    Private/Framework/HLSLFramework/FlattenIndexTests.cpp
    Private/Framework/HLSLFramework/ModelsTests.cpp
    Private/Framework/HLSLFramework/SectionHierarchyByteWriterTests.cpp
    Private/Framework/HLSLFramework/ProofOfConceptTests.cpp
    Private/Framework/HLSLFramework/PseudoConceptsTests.cpp
    Private/Framework/HLSLFramework/SectionManagementTests.cpp
    Private/Framework/HLSLFramework/ThreadDimensionsTests.cpp
    Private/Framework/HLSLFramework/ThreadIdRegistrationTests.cpp
    Private/Framework/HLSLFramework/TTLTypeTraitsTests.cpp
    Private/Framework/HLSLTypesTests.cpp
    Private/Framework/SectionsInHLSLProofOfConceptTests.cpp
    Private/Framework/ShaderTestDescriptorManagerTests.cpp
    Private/Framework/ShaderTestFixtureTests.cpp
    Private/Framework/TestDataBufferLayoutTests.cpp
    Private/Framework/TestDataBufferProcessorTests.cpp
    Private/Stats/ScopedObjectTests.cpp
    Private/Stats/StatSystemTests.cpp
    Private/TestUtilities/Noisy.cpp
    Private/TestUtilities/NoisyTests.cpp
    Private/Utility/AlignedOffsetTests.cpp
    Private/Utility/ConceptsTests.cpp
    Private/Utility/EnumReflectionTests.cpp
    Private/Utility/FunctionTraitsTests.cpp
    Private/Utility/LambdaTests.cpp
    Private/Utility/ObjectTests.cpp
    Private/Utility/PointerTests.cpp
    Private/Utility/TupleTests.cpp
    Private/Utility/TypeListTests.cpp
    Private/Utility/TypeTraitsTests.cpp
    Public/D3D12/Shader/ShaderCompilerTestsCommon.h
    Public/Framework/HLSLFramework/HLSLFrameworkTestsCommon.h
    Public/TestUtilities/Noisy.h
)

set(SHADER_SOURCE_REL_DIR "ShaderTests")
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shader)

target_sources(ShaderTestFrameworkTests PRIVATE ${SHADER_SOURCES})
target_sources(ShaderTestFrameworkTests PRIVATE ${SOURCES})

set_source_files_properties(${SHADER_SOURCES} PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")
set_target_properties(ShaderTestFrameworkTests PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ShaderTestFrameworkTests>" )
if (${STF_DISABLE_UNITY_STF})
    set_target_properties(ShaderTestFrameworkTests PROPERTIES UNITY_BUILD OFF)
endif()

# Hide away unity files in their own filter
source_group(UnityBuildFiles REGULAR_EXPRESSION ".+_cxx.cxx")
source_group(TREE ${SHADER_SOURCE_DIR} PREFIX "ShaderSources" FILES ${SHADER_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_compile_definitions(ShaderTestFrameworkTests PRIVATE SHADER_SRC="${SHADER_SOURCE_REL_DIR}" USE_PIX)
target_compile_options(ShaderTestFrameworkTests PRIVATE ${GLOBAL_COMPILE_FLAGS})
target_include_directories(ShaderTestFrameworkTests PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Public/)
target_link_libraries(ShaderTestFrameworkTests PRIVATE ShaderTestFramework)

target_add_asset_directory(ShaderTestFrameworkTests ${SHADER_SOURCE_DIR} "/${SHADER_SOURCE_REL_DIR}")
copy_all_dependent_assets(ShaderTestFrameworkTests)

add_catch2(ShaderTestFrameworkTests)

file(GENERATE 
    OUTPUT "${CMAKE_BINARY_DIR}/Run$<TARGET_NAME:ShaderTestFrameworkTests>$<CONFIG>.cmake"
    CONTENT 
    [=[ 
    cmake_minimum_required(VERSION 3.29)
    execute_process(
        COMMAND $<TARGET_FILE:ShaderTestFrameworkTests> --reporter xml --out ${CMAKE_BINARY_DIR}/build/ResultsCMake.xml --durations yes
        WORKING_DIRECTORY  $<TARGET_FILE_DIR:ShaderTestFrameworkTests>
        RESULT_VARIABLE RESULT_CODE
        COMMAND_ECHO STDOUT
    )
    
    cmake_language(EXIT ${RESULT_CODE})
    ]=]
    )