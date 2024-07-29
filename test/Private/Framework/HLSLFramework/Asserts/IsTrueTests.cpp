#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Asserts - IsTrue")
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

    const auto getDesc =
        [&testName]()
        {
            return
                ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/Asserts/IsTrue.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            };
        };

    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(getDesc()));
        }
        else
        {
            const auto result = fixture.RunTest(getDesc());
            REQUIRE_FALSE(result);
        }
    }
}