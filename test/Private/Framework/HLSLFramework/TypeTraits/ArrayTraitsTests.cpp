#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - ArrayTraits")
{
    auto testName = GENERATE
    (
        "GIVEN_BuiltInArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_NonArray_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_VectorType_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_Buffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_StructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWStructuredBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_ByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected",
        "GIVEN_RWByteAddressBuffer_WHEN_ArrayTraitsQueried_THEN_TraitsAsExpected"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/ArrayTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}