#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - StaticAssert")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"ConditionIsTrue", true},
                std::tuple{"ConditionIsTrueWithMessage", true},
                std::tuple{"ConditionIsTrueExpression", true},
                std::tuple{"ConditionIsFalse", false},
                std::tuple{"ConditionIsFalseWithMessage", false},
                std::tuple{"ConditionIsFalseExpression", false}
            }
        )
    );
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/StaticAssert/{}.hlsl", testName))));
    DYNAMIC_SECTION(testName)
    {
        const auto result = fixture.RunTest("Main", 1, 1, 1);
        if (shouldSucceed)
        {
            REQUIRE(result);
        }
        else
        {
            
            REQUIRE_FALSE(result);
        }
        
    }
}