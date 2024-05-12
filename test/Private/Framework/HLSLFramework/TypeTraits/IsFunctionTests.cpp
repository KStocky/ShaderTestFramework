#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - TypeTraits - IsFunction")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/TypeTraits/IsFunctionTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}