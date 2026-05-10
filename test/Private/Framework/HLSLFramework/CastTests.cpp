#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/AssertionsV1/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Cast")
{
    using namespace stf;
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

    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(
                AssertionsV1::ShaderTestFixture::RuntimeTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = fs::path(std::format("/Tests/Cast/{}.hlsl", testName))
                    },
                    .TestName = testName,
                    .ThreadGroupCount{1, 1, 1}
                })
            );
        }
        else
        {
            const auto result = fixture.RunTest(
                AssertionsV1::ShaderTestFixture::RuntimeTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = fs::path(std::format("/Tests/Cast/{}.hlsl", testName))
                    },
                    .TestName = testName,
                    .ThreadGroupCount{1, 1, 1}
                }
            );
            REQUIRE_FALSE(result);
        }
    }
}