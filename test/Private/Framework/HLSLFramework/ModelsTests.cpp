#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - Models")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/ModelsTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}