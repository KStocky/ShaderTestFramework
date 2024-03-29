#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - ThreadDimensionTests")
{
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

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/ThreadDimensionTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, dimX, dimY, dimZ));
    }
}