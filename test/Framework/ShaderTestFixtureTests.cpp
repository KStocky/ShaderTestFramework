
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
                std::tuple{"ThisTestShouldFail", false}
            }
        )
    );

    ShaderTestFixture::Desc FixtureDesc{};
    FixtureDesc.Source = std::string( R"(
                        #include "/Test/Public/ShaderTestFramework.hlsli"
                        [RootSignature(SHADER_TEST_RS)]
                        [numthreads(1,1,1)]
                        void ThisTestShouldPass(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            ShaderTestPrivate::Success();
                        }

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