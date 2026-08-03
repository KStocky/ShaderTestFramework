
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/AssertionsV1/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Matchers - Base Matchers")
{
    using namespace stf;

    const auto op = GENERATE(
        std::string{"Equals"},
        std::string{"LessThan"},
        std::string{"LessThanEqual"},
        std::string{"GreaterThanEqual"},
        std::string{"GreaterThan"}
    );

    GIVEN("Op is " << op)
    {
        const std::string testName = std::format("{}MatcherTests", op);

        REQUIRE(fixture.RunTest(
            AssertionsV1::ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path(std::format("/Tests/Matcher/{}MatcherTests.hlsl", op))
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}