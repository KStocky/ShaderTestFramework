#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - StaticAssert")
{
    auto [fileName, condition, shouldSucceed] = GENERATE
    (
        table<std::string, std::string, bool>
        (
            {
                std::tuple{"ConditionNoMessage", "true", true},
                std::tuple{"ConditionNoMessage", "false", false},
                std::tuple{"ConditionNoMessage", "42 == 42", true},
                std::tuple{"ConditionNoMessage", "42 != 42", false},
                std::tuple{"ConditionWithMessage", "true", true},
                std::tuple{"ConditionWithMessage", "false", false},
                std::tuple{"Global", "true", true},
                std::tuple{"Global", "false", false},
                std::tuple{"NonTemplatedStructMember", "true", true},
                std::tuple{"NonTemplatedStructMember", "false", false},
                std::tuple{"TemplatedStructMember", "true", true},
                std::tuple{"TemplatedStructMember", "false", false},
                std::tuple{"TemplatedStructMember", "ttl::is_same<T, uint>::value", true},
                std::tuple{"TemplatedStructMember", "ttl::is_same<T, T>::value", true},
                std::tuple{"TemplatedStructMemberInstantiated", "ttl::is_same<T, T>::value", true},
                std::tuple{"TemplatedStructMemberInstantiated", "ttl::is_same<T, uint>::value", false},
            }
        )
    );
    auto desc = CreateDescForHLSLFrameworkTest(fs::path(std::format("/Tests/StaticAssert/{}.hlsl", fileName)));
    desc.Defines.emplace_back("TEST_CONDITION", condition);
    ShaderTestFixture fixture(std::move(desc));
    DYNAMIC_SECTION(std::format("{} with condition: {}", fileName, condition))
    {
        const auto result = fixture.RunCompileTimeTest("Main");
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