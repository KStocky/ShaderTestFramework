
#include "Framework/ShaderTestFixture.h"
#include "D3D12/Shader/ShaderEnums.h"

#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace
{
    VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
    {
        fs::path shaderDir = fs::current_path();
        shaderDir += "/";
        shaderDir += SHADER_SRC;
        shaderDir += "/Framework/ShaderTestFixtureTests";

        return VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) };
    }

    ShaderTestFixture::Desc CreateDescForHLSLFrameworkTest(fs::path&& InPath, STF::AssertBufferLayout InAssertParams = {10, 1024})
    {
        ShaderTestFixture::Desc desc{};

        desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
        desc.HLSLVersion = EHLSLVersion::v2021;
        desc.Source = std::move(InPath);
        desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
        desc.AssertInfo = InAssertParams;
        return desc;
    }
}

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

SCENARIO("BasicShaderTests")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"ThisTestDoesNotExist", false},
                std::tuple{"ThisTestShouldPass", true},
                std::tuple{"ThisTestShouldFail", false},
            }
        )
    );
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/BasicShaderTests.hlsl")));
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

namespace ProofOfConcept
{
    static int CurrentTracker = 0;

    struct ScopedTracker
    {
        int ID;
        int ParentID;
        bool SectionHasBeenEntered;
        bool EnteredSubSection;
        bool HasSubSectionsRemaining;
        bool HasBeenInitialized;
    };

    static const int NumTrackers = 32;
    static ScopedTracker Trackers[NumTrackers];

    bool Init(int InID)
    {
        bool result = false;
        Trackers[InID].EnteredSubSection = false;
        if (!Trackers[InID].HasBeenInitialized)
        {
            Trackers[InID].SectionHasBeenEntered = false;
            Trackers[InID].HasSubSectionsRemaining = false;
            Trackers[InID].HasBeenInitialized = true;
            Trackers[InID].ID = InID;
            result = true;
        }
        else
        {
            result = !Trackers[InID].SectionHasBeenEntered || Trackers[InID].HasSubSectionsRemaining;
        }

        if (result && Trackers[InID].ID == 0)
        {
            Trackers[InID].ParentID = -1;
            Trackers[InID].SectionHasBeenEntered = true;
            CurrentTracker = 0;
        }
        return result;
    }

    bool TryEnterSection(int InID)
    {
        const bool needsExecution = Init(InID);
        const bool ourTurn = !Trackers[CurrentTracker].EnteredSubSection;
        if (needsExecution)
        {
            if (ourTurn)
            {
                Trackers[CurrentTracker].EnteredSubSection = true;
                Trackers[InID].ParentID = Trackers[CurrentTracker].ID;
                Trackers[InID].SectionHasBeenEntered = true;
                Trackers[CurrentTracker].HasSubSectionsRemaining = false;
                CurrentTracker = Trackers[InID].ID;
                return true;
            }
            else
            {

                Trackers[CurrentTracker].HasSubSectionsRemaining = true;
            }
        }
        return false;
    }

    void OnLeave()
    {
        CurrentTracker = Trackers[CurrentTracker].ParentID;
    }

    SCENARIO("SectionsInHLSLProofOfConceptTesting")
    {
        int num = 0;

        const int Section_0Num = 0;
        while (Init(Section_0Num))
        {
            ++num;

            const int Section_1Num = 1;
            if (TryEnterSection(Section_1Num))
            {
                ++num;

                OnLeave();
            }

            const int Section_2Num = 2;
            if (TryEnterSection(Section_2Num))
            {
                ++num;

                const int Section_3Num = 3;
                if (TryEnterSection(Section_3Num))
                {
                    ++num;
                    OnLeave();
                }
                OnLeave();
            }
        }

        REQUIRE(num == 5);
    }
}

SCENARIO("HLSLFrameworkTests - AssertBuffer - ResultProcessing")
{
    auto [testName, expected, numRecordedAsserts, numBytesData] = GENERATE
    (
        table<std::string, STF::TestRunResults, u32, u32>
        (
            {
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_ZeroAssertsMade_THEN_HasExpectedResults", STF::TestRunResults{ {}, 0, 0, uint3(1,1,1) }, 0, 0 },
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults", STF::TestRunResults{ {}, 2, 0, uint3(1,1,1) }, 0, 0 },
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_NonZeroFailedAssertsMade_THEN_HasExpectedResults", STF::TestRunResults{ {}, 0, 2, uint3(1,1,1) }, 0, 0 },
                std::tuple{ "GIVEN_ZeroAssertBuffer_WHEN_NonZeroSuccessfulAndFailedAssertsMade_THEN_HasExpectedResults", STF::TestRunResults{ {}, 2, 2, uint3(1,1,1) }, 0, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_ZeroAssertsMade_THEN_HasExpectedResults", STF::TestRunResults{ {}, 0, 0, uint3(1,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults", STF::TestRunResults{ {}, 2, 0, uint3(1,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_FailedAssert_THEN_HasExpectedResults", STF::TestRunResults{ {STF::FailedAssert{{}, STF::HLSLAssertMetaData{42, 0, 0, 0, 0, 0}}}, 0, 1, uint3(1,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_TwoFailedAssert_THEN_HasExpectedResults", STF::TestRunResults{ {STF::FailedAssert{{}, STF::HLSLAssertMetaData{42, 0, 0, 0, 0, 0}}, STF::FailedAssert{{}, STF::HLSLAssertMetaData{32, 0, 0, 0, 0, 0}}}, 0, 2, uint3(1,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacity_WHEN_FailedAssertWithLineId_THEN_HasExpectedResults", STF::TestRunResults{ {STF::FailedAssert{{}, STF::HLSLAssertMetaData{88, 0, 0, 0, 0, 0}}}, 0, 1, uint3(1,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacityWithFlatThreadId_WHEN_FailedAssert_THEN_HasExpectedResults", STF::TestRunResults{ {STF::FailedAssert{{}, STF::HLSLAssertMetaData{66, 12, 1, 0, 0, 0}}}, 0, 1, uint3(1,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacityWithFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults", STF::TestRunResults{ {STF::FailedAssert{{}, STF::HLSLAssertMetaData{66, 12, 2, 0, 0, 0}}}, 0, 1, uint3(24,1,1) }, 10, 0 },
                std::tuple{ "GIVEN_AssertInfoCapacityWithNonFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults", STF::TestRunResults{ {STF::FailedAssert{{}, STF::HLSLAssertMetaData{66, 4, 2, 0, 0, 0}}}, 0, 1, uint3(3,3,3) }, 10, 0 }
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/AssertBuffer/ResultsProcessing.hlsl"), { numRecordedAsserts, numBytesData }));
    fixture.TakeCapture();
    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}

SCENARIO("HLSLFrameworkTests - AssertBuffer - SizeTests")
{
    auto [testName, numRecordedFailedAsserts, numBytesAssertData] = GENERATE
    (
        table<std::string, u32, u32>
        (
            {
                std::tuple{"GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 0, 0},
                std::tuple{"GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 0},
                std::tuple{"GIVEN_FiveAssertsRecordedAnd100BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 100},
                std::tuple{"GIVEN_FiveAssertsRecordedAndNonMultipleOf4BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected", 5, 97}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/AssertBuffer/SizeTests.hlsl"), {numRecordedFailedAsserts, numBytesAssertData}));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - ByteWriter")
{
    auto testName = GENERATE
    (
        "GIVEN_FundamentalType_WHEN_HasWriterQueried_THEN_True",
        "GIVEN_NonFundamentalTypeWithNoWriter_WHEN_HasWriterQueried_THEN_False",
        "GIVEN_NonFundamentalTypeWithWriter_WHEN_HasWriterQueried_THEN_True",
        "GIVEN_FundamentalType_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned",
        "GIVEN_UIntBufferAndBool_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndUInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten",
        "GIVEN_UIntBufferAndFloat4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/ByteWriter/ByteWriterTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}

SCENARIO("HLSLFrameworkTests - Cast")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails", false},
                std::tuple{"GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/HLSLFrameworkTests/Cast/{}.hlsl", testName))));
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

SCENARIO("HLSLFrameworkTests - Asserts - AreEqual")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualFloat4_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualFloat4_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualNamedInts_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualNamedInts_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoNotEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/AreEqual.hlsl")));
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

SCENARIO("HLSLFrameworkTests - Asserts - NotEqual")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoEqualFloat4_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualFloat4_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoEqualNamedInts_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualNamedInts_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Fails", false},
                std::tuple{"GIVEN_TwoNotEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Fails", false},
                std::tuple{"GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/Asserts/NotEqual.hlsl")));
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
        "GIVEN_EmptySection_WHEN_Ran_THEN_NoAssertMade",
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce",
        "GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_Ran_THEN_ExpectedSubsectionEntryOccurs",
        "GIVEN_ThreeLevelsDeepSections_WHEN_Ran_THEN_ExpectedSubsectionEntryOccurs"
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

SCENARIO("HLSLFrameworkTests - Container")
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
        "GIVEN_SingleSubsection_WHEN_Ran_THEN_SectionsEntered2Times",
        "GIVEN_TwoSubsections_WHEN_Ran_THEN_SectionsEntered4Times",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_SectionsEntered5Times"
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