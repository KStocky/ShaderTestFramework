#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - VoidT")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/VoidTTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}