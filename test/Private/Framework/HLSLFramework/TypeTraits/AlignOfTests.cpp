#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - AlignOf")
{
    static constexpr std::string_view testName = "AllHaveExpectedAlignment";

    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/AlignOfTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest(testName));
}