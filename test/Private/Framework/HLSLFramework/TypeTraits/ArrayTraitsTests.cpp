#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - ArrayTraits")
{
    auto testName = GENERATE
    (
        "AllCompile"
    );

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/ArrayTraitsTests.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunCompileTimeTest(testName));
    }
}