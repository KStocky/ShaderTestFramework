#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - IsEnum")
{
    static constexpr std::string_view testName = "AllSucceed";
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/IsEnumTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest(testName));
}