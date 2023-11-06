
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
    //ShaderTestFixture Fixture("BasicTests.hlsl", TestParams{ {}, D3D_SHADER_MODEL_6_8 });
    //
    //auto testName = GENERATE("ThisTestShouldFail", "ThisTestShouldPass");
    //DYNAMIC_SECTION(testName)
    //{
    //    Fixture.RunTest(testName, 1, 1, 1);
    //}
}