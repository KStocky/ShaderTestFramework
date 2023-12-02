
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
            bool Other;
        };

        static StaticArrayTest globalArray[100];

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            for (int i = 0; i < 100; ++i)
            {
                STF::AreEqual(globalArray[i].Val, 0);
                STF::IsFalse(globalArray[i].Other);
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

SCENARIO("HLSLFrameworkTests - Cast")
{
    auto [testName, source, shouldSucceed] = GENERATE
    (
        table<std::string, std::string, bool>
        (
            {
                std::tuple
                {
                    "GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails",
                    std::string(
                    R"(
                    #include "/Test/Public/ShaderTestFramework.hlsli"

                    struct TestStruct
                    {
                        int Value;
                    };

                    [RootSignature(SHADER_TEST_RS)]
                    [numthreads(1,1,1)]
                    void GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
                    {
                        TestStruct test;
                        test.Value = 42;
                        int result = STF::Cast<int>(test);
                        STF::AreEqual(result, 42);
                    }
                    )"),
                    false
                },
                std::tuple
                {
                    "GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds",
                    std::string(
                    R"(
                    #include "/Test/Public/ShaderTestFramework.hlsli"

                    struct TestStruct
                    {
                        int Value;
                    };

                    namespace STF
                    {
                        template<>
                        int Cast<int, TestStruct>(TestStruct In)
                        {
                            return In.Value;
                        }
                    }

                    [RootSignature(SHADER_TEST_RS)]
                    [numthreads(1,1,1)]
                    void GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
                    {
                        TestStruct test;
                        test.Value = 42;
                        int result = STF::Cast<int>(test);
                        STF::AreEqual(result, 42);
                    }
                    )"),
                    true
                },
                std::tuple
                {
                    "GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails",
                    std::string(
                    R"(
                    #include "/Test/Public/ShaderTestFramework.hlsli"

                    struct TestStruct
                    {
                        int Value;
                    };

                    namespace STF
                    {
                        template<>
                        float Cast<float, TestStruct>(TestStruct In)
                        {
                            return (float)In.Value;
                        }
                    }

                    [RootSignature(SHADER_TEST_RS)]
                    [numthreads(1,1,1)]
                    void GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
                    {
                        TestStruct test;
                        test.Value = 42;
                        int result = STF::Cast<int>(test);
                        STF::AreEqual(result, 42);
                    }
                    )"),
                    false
                }
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::move(source);
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
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails", false},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Fails", false}
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
        };

        namespace STF
        {
            template<>
            bool Cast<bool, TestStruct>(TestStruct In)
            {
                return In.Value == 0;
            }
        }

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
        void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = 0;
            STF::IsTrue(test);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = 1;
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
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Fails", false},
                std::tuple{"GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Succeeds", true}
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
        };

        namespace STF
        {
            template<>
            bool Cast<bool, TestStruct>(TestStruct In)
            {
                return In.Value == 0;
            }
        }

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
        void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = 0;
            STF::IsFalse(test);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            TestStruct test;
            test.Value = 1;
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

SCENARIO("HLSLFrameworkTests - Macros - SectionVarCreation")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero", true},
                std::tuple{"GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected", true},
                std::tuple{"GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected", true}
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
        void GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(0, ShaderTestPrivate::NextSectionID);
            STF_CREATE_SECTION_VAR;

            STF::AreEqual(1, ShaderTestPrivate::NextSectionID);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(0, ShaderTestPrivate::NextSectionID);
            STF_CREATE_SECTION_VAR;
            STF_CREATE_SECTION_VAR;
            STF::AreEqual(2, ShaderTestPrivate::NextSectionID);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            STF::AreEqual(0, ShaderTestPrivate::NextSectionID);
            
            for (int i = 0; i < 3; ++i)
            {
                STF_CREATE_SECTION_VAR;
                STF_CREATE_SECTION_VAR;
            }

            STF::AreEqual(2, ShaderTestPrivate::NextSectionID);
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

SCENARIO("HLSLFrameworkTests - SectionManagement")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce", true},
                std::tuple{"GIVEN_SingleSubsection_WHEN_Ran_THEN_SectionsEntered2Times", true},
                std::tuple{"GIVEN_TwoSubsections_WHEN_Ran_THEN_SectionsEntered4Times", true},
                std::tuple{"GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_SectionsEntered5Times", true}
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::string(
        R"(
        #include "/Test/Public/ShaderTestFramework.hlsli"

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            int num = 0;
            static const int Section_0Num = 0;
            while (ShaderTestPrivate::TryEnterSection(Section_0Num))
            {
                ++num;
            }  
    
            STF::AreEqual(1, num);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_SingleSubsection_WHEN_Ran_THEN_SectionsEntered2Times(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            int num = 0;
            static const int Section_0Num = 0;
            while (ShaderTestPrivate::TryEnterSection(Section_0Num))
            {
                ++num;
                static const int Section_1Num = 1;
                if (ShaderTestPrivate::TryEnterSection(Section_1Num))
                {
                    ++num;

                    ShaderTestPrivate::OnLeave();
                }
            }  
    
            STF::AreEqual(2, num);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoSubsections_WHEN_Ran_THEN_SectionsEntered4Times(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            int num = 0;
            static const int Section_0Num = 0;
            while (ShaderTestPrivate::TryEnterSection(0, Section_0Num))
            {
                ++num;
                static const int Section_1Num = 1;
                if (ShaderTestPrivate::TryEnterSection(0, Section_1Num))
                {
                    ++num;
                    ShaderTestPrivate::OnLeave(0);
                }
                
                static const int Section_2Num = 2;
                if (ShaderTestPrivate::TryEnterSection(0, Section_2Num))
                {
                    ++num;
                    ShaderTestPrivate::OnLeave(0);
                }
            }  
    
            STF::AreEqual(4, num);
        }

        [RootSignature(SHADER_TEST_RS)]
        [numthreads(1,1,1)]
        void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_SectionsEntered5Times(uint3 DispatchThreadId : SV_DispatchThreadID)
        {
            int num = 0;
            const uint id = 0;
            static const int Section_0Num = 0;
            while (ShaderTestPrivate::TryEnterSection(id, Section_0Num))
            {
                ++num;

                static const int Section_1Num = 1;
                if (ShaderTestPrivate::TryEnterSection(id, Section_1Num))
                {
                    ++num;

                    ShaderTestPrivate::OnLeave(id);
                }

                static const int Section_2Num = 2;
                if (ShaderTestPrivate::TryEnterSection(id, Section_2Num))
                {
                    ++num;

                    static const int Section_3Num = 3;
                    if (ShaderTestPrivate::TryEnterSection(id, Section_3Num))
                    {
                        ++num;
                        ShaderTestPrivate::OnLeave(id);
                    }
                    ShaderTestPrivate::OnLeave(id);
                }
            }  
    
            STF::AreEqual(5, num);
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

namespace DebuggingSections
{
    static int NextSectionID = 0;
    static const int NumSections = 32;

    static int CurrentSectionID = 0;

    struct ScenarioSectionInfo
    {
        int ParentID;
        bool HasBeenEntered;
        bool HasSubsectionBeenEntered;
        bool HasUnenteredSubsections;
    };

    static ScenarioSectionInfo Sections[NumSections];

    bool TryEnterSection(int InID)
    {
        const bool shouldEnter = !Sections[InID].HasBeenEntered || Sections[InID].HasUnenteredSubsections;

        if (shouldEnter)
        {
            if (InID == 0)
            {
                CurrentSectionID = 0;
                Sections[InID].HasBeenEntered = true;
                Sections[InID].HasSubsectionBeenEntered = false;

                return true;
            }
            else
            {
                const bool ourTurn = !Sections[CurrentSectionID].HasSubsectionBeenEntered;
                if (ourTurn)
                {
                    Sections[CurrentSectionID].HasSubsectionBeenEntered = true;
                    Sections[CurrentSectionID].HasUnenteredSubsections = false;
                    Sections[InID].ParentID = CurrentSectionID;
                    Sections[InID].HasBeenEntered = true;
                    CurrentSectionID = InID;
                    return true;
                }
                else
                {
                    Sections[CurrentSectionID].HasUnenteredSubsections = true;
                }
            }
        }

        return false;
    }

    void OnLeave()
    {
        CurrentSectionID = Sections[CurrentSectionID].ParentID;
    }

    SCENARIO("DebuggingSections")
    {
        int num = 0;
        static const int Section_0Num = 0;
        while (TryEnterSection(Section_0Num))
        {
            ++num;

            static const int Section_1Num = 1;
            if (TryEnterSection(Section_1Num))
            {
                ++num;

                OnLeave();
            }

            static const int Section_2Num = 2;
            if (TryEnterSection(Section_2Num))
            {
                ++num;

                static const int Section_3Num = 3;
                if (TryEnterSection(Section_3Num))
                {
                    ++num;
                    OnLeave();
                }
                OnLeave();
            }
        }

        REQUIRE(5 == num);
    }
}

SCENARIO("HLSLFrameworkTests - Bugs")
{
    auto [testName, code, shouldSucceed] = GENERATE
    (
        table<std::string, std::string, bool>
        (
            {
                std::tuple
                {
                    "GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed",
                    std::string(
                    R"SHADER(
                #define SHADER_TEST_RS \
                "RootFlags(" \
                    "DENY_VERTEX_SHADER_ROOT_ACCESS |" \
                    "DENY_HULL_SHADER_ROOT_ACCESS |" \
                    "DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
                    "DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
                    "DENY_PIXEL_SHADER_ROOT_ACCESS |" \
                    "DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |" \
                    "DENY_MESH_SHADER_ROOT_ACCESS |" \
                    "CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED" \
                ")," \
                "RootConstants(" \
                    "num32BitConstants=2," \
                    "b0)"
                
                    struct StaticArrayTest
                    {
                        int Val;
                        bool Other;
                        bool Other1;
                        bool Hi;
                    };
                
                    static const int Num = 100;
                    static StaticArrayTest globalArray[Num];
                
                    bool Test(int InIndex)
                    {
                        StaticArrayTest t = globalArray[InIndex];
                        return t.Val == 0 && !t.Other && !t.Other1 && !t.Hi;
                    }
                
                    [RootSignature(SHADER_TEST_RS)]
                    [numthreads(1,1,1)]
                    void GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed(uint3 DispatchThreadId : SV_DispatchThreadID)
                    {
                        bool pass = true;
                        for (int i = 0; i < Num; ++i)
                        {
                            pass = pass && Test(i);
                        }
                
                        if (!pass)
                        {
                            RWByteAddressBuffer buf = ResourceDescriptorHeap[0];
                            uint failIndex;
                            buf.InterlockedAdd(4, 1, failIndex);
                        }
                    }
                    )SHADER"),
                    true
                },
                //std::tuple
                //{
                //    "GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail",
                //    std::string(
                //    R"SHADER(
                //    
                //    #define RS \
                //    "RootFlags(" \
                //        "DENY_VERTEX_SHADER_ROOT_ACCESS |" \
                //        "DENY_HULL_SHADER_ROOT_ACCESS |" \
                //        "DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
                //        "DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
                //        "DENY_PIXEL_SHADER_ROOT_ACCESS |" \
                //        "DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |" \
                //        "DENY_MESH_SHADER_ROOT_ACCESS |" \
                //        "CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED" \
                //    ")," \
                //    "RootConstants(" \
                //        "num32BitConstants=2," \
                //        "b0" \
                //    ")"
                //
                //    static bool StaticBool[1];
                //
                //    [RootSignature(RS)]
                //    [numthreads(1,1,1)]
                //    void GIVEN_WarpAndNoOptimizations_WHEN_ThisIsRun_THEN_ItWillSomeTimesFail(uint3 DispatchThreadId : SV_DispatchThreadID)
                //    {
                //        if(StaticBool[0])
                //        {
                //            RWByteAddressBuffer results = ResourceDescriptorHeap[0];
                //            uint old;
                //            results.InterlockedAdd(4, 1, old);
                //        }
                //
                //        for (int i = 0; i < 1; ++i)
                //        {
                //            static int value = 0;
                //            static int name = value;
                //            StaticBool[name] = true;
                //        }
                //    }
                //    )SHADER"),
                //    true
                //},
                std::tuple
                {
                    "GIVEN_Object_WHEN_ConversionOperatorCalled_THEN_Fails",
                    std::string(
                    R"SHADER(
                    #include "/Test/Public/ShaderTestFramework.hlsli"

                    struct TestStruct
                    {
                        int Val;

                        bool Test()
                        {
                            return Val == 0;
                        }

                        operator bool()
                        {
                            return Test();
                        }
                    };

                    [RootSignature(SHADER_TEST_RS)]
                    [numthreads(1,1,1)]
                    void GIVEN_StaticObject_WHEN_ConversionOperatorCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
                    {
                        TestStruct test;
                        test.Val = 0;
                        const bool result = (bool)test;          
                        STF::IsTrue(result);
                    }
                    )SHADER"),
                    false
                }
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    FixtureDesc.HLSLVersion = EHLSLVersion::v2021;
    FixtureDesc.Source = std::move(code);
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