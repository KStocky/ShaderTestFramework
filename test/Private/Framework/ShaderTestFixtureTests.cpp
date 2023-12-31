
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <sstream>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

SCENARIO("ShaderTestFixtureTests")
{
    GIVEN("Default Constructed Fixture")
    {
        ShaderTestFixture test{ {} };
        WHEN("Queried for Validity")
        {
            THEN("Is Valid")
            {
                REQUIRE(test.IsValid());
            }
        }

        WHEN("Queried for AgilitySDK")
        {
            THEN("Has Agility")
            {
                REQUIRE(test.IsUsingAgilitySDK());
            }
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - IsTrue")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TrueNonLiteral_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TrueExpression_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/IsTrue.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - Fail")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails", false},
                std::tuple{"GIVEN_Empty_WHEN_Ran_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/Fail.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Asserts - IsFalse")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TrueNonLiteral_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TrueExpression_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Fails", false},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/IsFalse.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Macros - AssertMacro")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/AssertMacro.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - Macros - NumArgs")
{
    auto testName = GENERATE
    (
        "GIVEN_ZeroArgs_WHEN_Counted_THEN_ReturnsZero",
        "GIVEN_OneArg_WHEN_Counted_THEN_ReturnsOne",
        "GIVEN_TenArgs_WHEN_Counted_THEN_ReturnsTen"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/NumArgs.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Macros - SectionVarCreation")
{
    auto testName = GENERATE
    (
        "GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero",
        "GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected",
        "GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/SectionVarCreation.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}


SCENARIO("HLSLFrameworkTests - Macros - SCENARIO")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade",
        "GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce",
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_ScenarioWithoutId_WHEN_Ran_THEN_IdIsNone",
        "GIVEN_ScenarioWithDispatchThreadId_WHEN_Ran_THEN_IdIsInt3",
        "GIVEN_ScenarioWithIntId_WHEN_Ran_THEN_IdIsInt"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/Scenario.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Macros - SECTIONS")
{
    auto testName = GENERATE
    (
        "GIVEN_EmptySection_WHEN_RanUsingBeginEnd_THEN_NoAssertMade",
        "GIVEN_EmptySection_WHEN_RanUsingSingle_THEN_NoAssertMade",
        "GIVEN_SingleSection_WHEN_RanUsingBeginEnd_THEN_SectionsEnteredOnce",
        "GIVEN_SingleSection_WHEN_RanUsingSingle_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_RanUsingBeginEnd_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSections_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingBeginEnd_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_RanUsingBeginEnd_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_ThreeLevelsDeepSections_WHEN_RanUsingBeginEnd_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_ThreeLevelsDeepSections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Macros/Sections.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - ProofOfConcept")
{
    auto testName = GENERATE
    (
        "GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent",
        "GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed",
        "SectionTest",
        "GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported",
        "GIVEN_SomeTypesWithAndWithoutASpecializations_WHEN_ApplyFuncCalledOnThem_THEN_ExpectedResultsReturned",
        "VariadicMacroOverloading"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/ProofOfConcept/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Container_wrapper")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected", true},
                std::tuple{"GIVEN_IntArray_WHEN_LoadCalled_THEN_ReturnsExpectedValue", true},
                std::tuple{"GIVEN_IntArray_WHEN_StoreCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_StoreCalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store2CalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store3CalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store4Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store4CalledWithDifferentType_THEN_Fails", false }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/Container/ArrayTests/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}

SCENARIO("HLSLFrameworkTests - ThreadDimensionTests")
{
    auto [testName, dimX, dimY, dimZ] = GENERATE
    (
        table<std::string, u32, u32, u32>
        (
            {
                std::tuple{"GIVEN_SingleThreadDispatched_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 1, 1, 1},
                std::tuple{"GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 10, 10, 10},
                std::tuple{"GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 1, 1, 1},
                std::tuple{"GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 2, 2, 2}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/ThreadDimensionTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, dimX, dimY, dimZ));
    }
}

SCENARIO("HLSLFrameworkTests - ThreadIdRegistrationTests")
{
    auto [testName, dimX, dimY, dimZ] = GENERATE
    (
        table<std::string, u32, u32, u32>
        (
            {
                std::tuple{"GIVEN_SingleThreadDispatched_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1},
                std::tuple{"GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 10, 10, 10},
                std::tuple{"GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1},
                std::tuple{"GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect", 2, 2, 2},
                std::tuple{"GIVEN_ThreadIDGivenAsUint_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1},
                std::tuple{"GIVEN_ThreadIdNotRegistered_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect", 1, 1, 1}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/ThreadIdRegistrationTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, dimX, dimY, dimZ));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - ArrayTraits")
{
    auto testName = GENERATE
    (
        "GIVEN_BuiltInArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_NonArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_VectorType_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_Buffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_StructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWStructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_ByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/ArrayTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - ContainerTraits")
{
    auto testName = GENERATE
    (
        "GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_StructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/ContainerTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - FundamentalTypeTraits")
{
    auto testName = GENERATE
    (
        "AllShouldPass"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/FundamentalTypeTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - FundamentalType")
{
    auto testName = GENERATE
    (
        "AllShouldPass"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/FundamentalTypeTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - TypeTraits - TypeID")
{
    auto testName = GENERATE
    (
        "NoDuplicateTypeIDs"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/TypeIDTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - SectionManagement")
{
    auto testName = GENERATE
    (
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_SingleSubsection_WHEN_RanUsingIf_THEN_SectionsEntered2Times",
        "GIVEN_TwoSubsections_WHEN_RanUsingIf_THEN_SectionsEntered4Times",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingIf_THEN_SectionsEntered5Times",
        "GIVEN_SingleSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered2Times",
        "GIVEN_TwoSubsections_WHEN_RanUsingWhile_THEN_SectionsEntered4Times",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered5Times"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/SectionManagement.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - FlattenIndex")
{
    auto testName = GENERATE
    (
        "GIVEN_UnitCubeAndZeroIndex_WHEN_Flattened_THEN_ReturnsZero",
        "GIVEN_CubeSide10AndZeroIndex_WHEN_Flattened_THEN_ReturnsZero",
        "GIVEN_CubeSide10AndIndicesDifferentByOneInX_WHEN_BothFlattened_THEN_DifferenceIsOne",
        "GIVEN_CubeSide10AndIndicesDifferentByOneInY_WHEN_BothFlattened_THEN_DifferenceIsTen",
        "GIVEN_CubeSide10AndIndicesDifferentByOneInZ_WHEN_BothFlattened_THEN_DifferenceIs100",
        "GIVEN_CubeSide3_WHEN_IndicesIncrementedInXThenYThenZ_AND_WHEN_Flattened_THEN_DifferenceIs1"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/FlattenIndex.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Bugs")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                //std::tuple
                //{
                //    "GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail",
                //    true
                //},
                std::tuple
                {
                    "GIVEN_Object_WHEN_ConversionOperatorCalled_THEN_Fails",
                    false
                }
            }
        )
    );


    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/Bugs/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
    }
}