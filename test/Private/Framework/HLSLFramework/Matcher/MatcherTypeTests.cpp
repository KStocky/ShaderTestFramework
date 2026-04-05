
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - Matchers - MatcherType")
{
    using namespace stf;
    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    REQUIRE(fixture.RunCompileTimeTest(
        ShaderCompileTestDesc
        {
            .CompilationEnv
            {
                .Source = fs::path("/Tests/Matcher/MatcherTypeTests.hlsl")
            },
            .TestName = "Matcher Type Tests"
        }
    ));
}