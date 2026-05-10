#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/AssertionsV1/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - Concepts")
{
    using namespace stf;
    AssertionsV1::ShaderTestFixture fixture(
        AssertionsV1::ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    REQUIRE(fixture.RunCompileTimeTest(
        ShaderCompileTestDesc
        {
            .CompilationEnv
            {
                .Source = fs::path("/Tests/ConceptsTests.hlsl")
            },
            .TestName = "Pseudo Concepts Traits"
        }
    ));

}