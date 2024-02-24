#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Memory - Zero")
{
    auto testName = GENERATE
    (
        "GIVEN_ObjectIsInitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed",
        "GIVEN_ObjectIsUnitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed",
        "GIVEN_NoObjectAlreadyExists_WHEN_Zeroed_THEN_AllMembersOfReturnedObjectAreZeroed"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/Memory/ZeroTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}