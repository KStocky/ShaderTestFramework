#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - ThreadDimensionTests")
{
    using namespace stf;
    auto [testName, dimX, dimY, dimZ] = GENERATE
    (
        table<std::string, u32, u32, u32>
        (
            {
                std::tuple{"GIVEN_SingleThreadDispatched_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 1, 1, 1},
                std::tuple{"GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 10, 10, 10},
                std::tuple{"GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 1, 1, 1},
                std::tuple{"GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected", 2, 2, 2}
            }
        )
    );

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/ThreadDimensionTests.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{dimX, dimY, dimZ}
            }
        ));
    }
}