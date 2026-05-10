#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/AssertionsV1/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - SectionHierarchy - ByteWriter")
{
    using namespace stf;
    AssertionsV1::ShaderTestFixture fixture(
        AssertionsV1::ShaderTestFixture::FixtureDesc
        {
            .Mappings{GetTestVirtualDirectoryMapping()}
        }
    );
    
    REQUIRE(fixture.RunTest(
        AssertionsV1::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                .Source = fs::path("/Tests/SectionHierarchyByteWriterTests.hlsl")
            },
            .TestName = "SectionHierarchyByteWriterTests",
            .ThreadGroupCount{1, 1, 1},
            .PerTestData
            {
                .NumFailedAsserts = 100,
                .NumBytesAssertData = 1024,
                .NumStrings = 100,
                .NumBytesStringData = 6400,
                .NumSections = 100
            }
        })
    );
}