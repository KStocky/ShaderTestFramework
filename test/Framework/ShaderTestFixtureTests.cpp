
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

struct TestParams
{
    std::vector<std::string> Flags;
    D3D_SHADER_MODEL ShaderModel;
};

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
            ShaderTestPrivate::AddError(0,0,0,0);
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
                ShaderTestPrivate::AddError(0,0,0,0);
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
                    ShaderTestPrivate::AddError(0,0,0,0);
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
                ShaderTestPrivate::AddError(0,0,0,0);
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

SCENARIO("HLSLFrameworkTests")
{
    //SKIP("Not implemented yet");
    //SECTION("Test")
    //{
    //    auto [testName, shouldSucceed] = GENERATE
    //    (
    //        table<std::string, bool>
    //        (
    //            {
    //                std::tuple{"GIVEN_TheTestShouldPass_WHEN_Run_THEN_TheTestPasses", false},
    //            }
    //            )
    //    );
    //
    //    ShaderTestFixture::Desc FixtureDesc{};
    //    FixtureDesc.Source = std::string(R"(
    //                    #include "/Test/Public/ShaderTestFramework.hlsli"
    //                    [RootSignature(SHADER_TEST_RS)]
    //                    [numthreads(1,1,1)]
    //                    void GIVEN_TheTestShouldPass_WHEN_Run_THEN_TheTestPasses(uint3 DispatchThreadId : SV_DispatchThreadID)
    //                    {
    //                        Assert::Equal(4, 4);
    //                        Assert::NotEqual(42, 5);
    //                        Assert::NearlyEqual(21.0001, 21.0);
    //                    }
    //                    )");
    //    ShaderTestFixture Fixture(std::move(FixtureDesc));
    //    Fixture.TakeCapture();
    //    DYNAMIC_SECTION(testName)
    //    {
    //        if (shouldSucceed)
    //        {
    //            REQUIRE(Fixture.RunTest(testName, 1, 1, 1));
    //        }
    //        else
    //        {
    //            const auto result = Fixture.RunTest(testName, 1, 1, 1);
    //            REQUIRE(!result);
    //        }
    //
    //    }
    //}
}