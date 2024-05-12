#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - IsInvocableFunction")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/IsInvocableFunctionTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}