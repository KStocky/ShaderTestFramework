#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - DeclVal")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/DeclValTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}