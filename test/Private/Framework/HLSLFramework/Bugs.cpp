#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Bugs")
{
    using namespace stf;
    auto [testName, testFile, shouldSucceed] = GENERATE
    (
        table<std::string, std::string, bool>
        (
            {
                std::tuple
                {
                    "GIVEN_Object_WHEN_ConversionOperatorCalled_THEN_Fails",
                    "ConversionOperator",
                    false
                }
            }
        )
    );

    DYNAMIC_SECTION(testName)
    {

        if (shouldSucceed)
        {
            
            REQUIRE(fixture.RunTest(
                ShaderTestFixture::RuntimeTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = fs::path(std::format("/Tests/Bugs/{}.hlsl", testFile))
                    },
                    .TestName = testName,
                    .ThreadGroupCount{1, 1, 1}
                })
            );
        }
        else
        {
            const auto result = fixture.RunTest(
                ShaderTestFixture::RuntimeTestDesc
                {
                    .CompilationEnv
                    {
                        .Source = fs::path(std::format("/Tests/Bugs/{}.hlsl", testFile))
                    },
                    .TestName = testName,
                    .ThreadGroupCount{1, 1, 1}
                });
            REQUIRE_FALSE(result);
        }
    }
}