#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - FundamentalTypeTraits")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/FundamentalTypeTraitsTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}