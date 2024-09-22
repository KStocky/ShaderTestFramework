#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - Concepts")
{
    using namespace stf;
    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    REQUIRE(fixture.RunCompileTimeTest(
        ShaderTestFixture::CompileTestDesc
        {
            .CompilationEnv
            {
                .Source = fs::path("/Tests/ConceptsTests.hlsl")
            },
            .TestName = "Pseudo Concepts Traits"
        }
    ));

}