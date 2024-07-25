#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - Macros - STAMP")
{
    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    REQUIRE(fixture.RunTest(
        ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                .Source = fs::path("/Tests/Macros/Stamp.hlsl")
            },
            .TestName = "StampTests",
            .ThreadGroupCount{1, 1, 1}
        }
    ));
}