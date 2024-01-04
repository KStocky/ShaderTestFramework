#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

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

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/Asserts/Fail.hlsl")));
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