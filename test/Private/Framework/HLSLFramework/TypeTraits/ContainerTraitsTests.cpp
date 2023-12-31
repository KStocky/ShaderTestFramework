#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - ContainerTraits")
{
    auto testName = GENERATE
    (
        "GIVEN_BuiltInArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_NonArray_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_VectorType_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_Buffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_StructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWStructuredBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_ByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWByteAddressBuffer_WHEN_ContainerTraitsQueried_THEN_TraitsAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/HLSLFrameworkTests/TypeTraits/ContainerTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}