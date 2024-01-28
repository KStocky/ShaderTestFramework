#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - ArrayTraits")
{
    static constexpr std::string_view testName = "AllCompile";

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/ArrayTraitsTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest(testName));
}