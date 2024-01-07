
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Macros - NumArgs")
{
    auto testName = GENERATE
    (
        "GIVEN_ZeroArgs_WHEN_Counted_THEN_ReturnsZero",
        "GIVEN_OneArg_WHEN_Counted_THEN_ReturnsOne",
        "GIVEN_TenArgs_WHEN_Counted_THEN_ReturnsTen"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/Macros/NumArgs.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}