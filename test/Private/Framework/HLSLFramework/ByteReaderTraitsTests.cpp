#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - ByteReaderTraits")
{
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/Tests/ByteReaderTraits/ByteReaderTraitsTests.hlsl")));
    REQUIRE(fixture.RunCompileTimeTest());
}