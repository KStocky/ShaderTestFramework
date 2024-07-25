#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Container_wrapper")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected", true},
                std::tuple{"GIVEN_IntArray_WHEN_LoadCalled_THEN_ReturnsExpectedValue", true},
                std::tuple{"GIVEN_IntArray_WHEN_StoreCalled_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_StoreCalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store2CalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store3Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store3CalledWithDifferentType_THEN_Fails", false },
                std::tuple{"GIVEN_IntArray_WHEN_Store4Called_THEN_Succeeds", true},
                std::tuple{"GIVEN_IntArray_WHEN_Store4CalledWithDifferentType_THEN_Fails", false }
            }
        )
    );

    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    const auto getDesc =
        [&testName]()
        {
            return
                ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path(std::format("/Tests/Container/ArrayTests/{}.hlsl", testName))
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