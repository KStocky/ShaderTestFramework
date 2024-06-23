#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Memory - AlignedOffset")
{
    auto testName = GENERATE
    (
        "GIVEN_IndexIsAlreadyAligned_WHEN_Aligned_THEN_ReturnsSameIndex",
        "GIVEN_IndexIsZero_WHEN_Aligned_THEN_ReturnsZero",
        "GIVEN_IndexIsOneMinusAlignment_WHEN_Aligned_THEN_Alignment",
        "GIVEN_IndexIsNotAligned_WHEN_Aligned_THEN_ReturnsNextMultipleOfAlignment"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/Memory/AlignedOffsetTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(!fixture.RunTest(testName, 1, 1, 1));
    }
}