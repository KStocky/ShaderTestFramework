
#include "Framework/ShaderTestFixture.h"
#include "D3D12/Shader/IncludeHandler.h"
#include "D3D12/Shader/ShaderEnums.h"

#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("ShaderTestFixtureTests")
{
    GIVEN("Default Constructed Fixture")
    {
        ShaderTestFixture Test{ {} };
        WHEN("Queried for Validity")
        {
            THEN("Is Valid")
            {
                REQUIRE(Test.IsValid());
            }
        }

        WHEN("Queried for AgilitySDK")
        {
            THEN("Has Agility")
            {
                REQUIRE(Test.IsUsingAgilitySDK());
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

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string( 
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void ThisTestShouldPass(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            ShaderTestPrivate::Success();
        }
                        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void ThisTestShouldFail(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            ShaderTestPrivate::AddError();
        }
        )");
    ShaderTestFixture Fixture(std::move(FixtureDesc));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = Fixture.RunTest(testName, 1, 1, 1);
            REQUIRE(!result);
        }
        
    }
}

SCENARIO("ShaderFrameworkHLSLProofOfConceptTests")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent", true},
                std::tuple{"GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed", true},
                std::tuple{"SectionTest", true}
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string(
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            int a = __COUNTER__;
            int b = __COUNTER__;
            if (a == 0 && b == 1)
            {
                ShaderTestPrivate::Success();
            }
            else
            {
                ShaderTestPrivate::AddError();
            }
        }

        struct StaticArrayTest
        {
            int Val;
        };

        static StaticArrayTest globalArray[24];

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            for (int i = 0; i < 24; ++i)
            {
                if (globalArray[i].Val == 0)
                {
                    ShaderTestPrivate::Success();
                }
                else
                {
                    ShaderTestPrivate::AddError();
                }
            }
        }


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

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void SectionTest(uint3 DispatchThreadId : SV_DispatchThreadID)
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
    
            if (num == 5)
            {
                ShaderTestPrivate::Success();
            }
            else
            {
                ShaderTestPrivate::AddError();
            }
        }
        )");
    ShaderTestFixture Fixture(std::move(FixtureDesc));
    Fixture.TakeCapture();
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = Fixture.RunTest(testName, 1, 1, 1);
            REQUIRE(!result);
        }

    }
}

namespace
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

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string(
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"

        struct TestStruct
        {
            int Value;
            bool operator==(TestStruct InOther)
            {
                return Value == InOther.Value;
            }
        };

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(4, 4);
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(4, 5);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualFloat4_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(float4(1.0, 2.5, 3.5, 5.75), float4(1.0, 2.5, 3.5, 5.75));
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualFloat4_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(float4(1.0, 2.5, 3.5, 5.75), float4(2.0, 2.5, 3.5, 5.75));
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualNamedInts_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const int expected = 4;
            STF::AreEqual(4, expected);
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualNamedInts_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const int expected = 5;
            STF::AreEqual(4, expected);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const float4 expected = float4(1.0, 2.5, 3.5, 5.75);
            STF::AreEqual(float4(1.0, 2.5, 3.5, 5.75), expected);
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const float4 expected = float4(2.0, 2.5, 3.5, 5.75);
            STF::AreEqual(float4(1.0, 2.5, 3.5, 5.75), expected);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 42;
            STF::AreEqual(left, right);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 43;
            STF::AreEqual(left, right);
        }
        )");
    ShaderTestFixture Fixture(std::move(FixtureDesc));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = Fixture.RunTest(testName, 1, 1, 1);
            REQUIRE(!result);
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

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string(
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"

        struct TestStruct
        {
            int Value;
            bool operator!=(TestStruct InOther)
            {
                return Value != InOther.Value;
            }
        };

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::NotEqual(4, 4);
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::NotEqual(4, 5);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualFloat4_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::NotEqual(float4(1.0, 2.5, 3.5, 5.75), float4(1.0, 2.5, 3.5, 5.75));
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualFloat4_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::NotEqual(float4(1.0, 2.5, 3.5, 5.75), float4(2.0, 2.5, 3.5, 5.75));
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualNamedInts_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const int expected = 4;
            STF::NotEqual(4, expected);
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualNamedInts_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const int expected = 5;
            STF::NotEqual(4, expected);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const float4 expected = float4(1.0, 2.5, 3.5, 5.75);
            STF::NotEqual(float4(1.0, 2.5, 3.5, 5.75), expected);
        }
        
        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoNotEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            static const float4 expected = float4(2.0, 2.5, 3.5, 5.75);
            STF::NotEqual(float4(1.0, 2.5, 3.5, 5.75), expected);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 42;
            STF::NotEqual(left, right);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct left;
            TestStruct right;

            left.Value = 42;
            right.Value = 43;
            STF::NotEqual(left, right);
        }
        )");
    ShaderTestFixture Fixture(std::move(FixtureDesc));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = Fixture.RunTest(testName, 1, 1, 1);
            REQUIRE(!result);
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
                std::tuple{"GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsTrue_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsFalse_THEN_Fails", false}
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string(
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"

        struct TestStruct
        {
            bool Value;
            operator bool()
            {
                return Value;
            }
        };

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsTrue(true);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TrueNonLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            const bool value = true;
            STF::IsTrue(value);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TrueExpression_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsTrue(2 == 2);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsTrue(false);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            const bool value = false;
            STF::IsTrue(value);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsTrue(2 == 3);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsTrue_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = true;
            STF::IsTrue(test);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsFalse_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = false;
            STF::IsTrue(test);
        }
        )");
    ShaderTestFixture Fixture(std::move(FixtureDesc));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = Fixture.RunTest(testName, 1, 1, 1);
            REQUIRE(!result);
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
                std::tuple{"GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsTrue_THEN_Fails", false},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsFalse_THEN_Succeeds", true}
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string(
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"

        struct TestStruct
        {
            bool Value;
            operator bool()
            {
                return Value;
            }
        };

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsFalse(true);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TrueNonLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            const bool value = true;
            STF::IsFalse(value);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TrueExpression_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsFalse(2 == 2);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsFalse(false);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            const bool value = false;
            STF::IsFalse(value);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::IsFalse(2 == 3);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsTrue_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = true;
            STF::IsFalse(test);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_ObjectWithOperatorBool_WHEN_ObjectsIsFalse_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = false;
            STF::IsFalse(test);
        }
        )");
    ShaderTestFixture Fixture(std::move(FixtureDesc));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = Fixture.RunTest(testName, 1, 1, 1);
            REQUIRE(!result);
        }
    }
}