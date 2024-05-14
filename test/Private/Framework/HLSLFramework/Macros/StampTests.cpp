#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - Macros - STAMP")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/Macros/Stamp.hlsl")));
    REQUIRE(fixture.RunTest("StampTests", 1, 1, 1));
}