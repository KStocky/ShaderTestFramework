#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>

SCENARIO("HLSLFrameworkTests - SectionHierarchy - ByteWriter")
{
    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{GetTestVirtualDirectoryMapping()}
        }
    );

    REQUIRE(fixture.RunTest(
        ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                .Source = fs::path("/Tests/SectionHierarchyByteWriterTests.hlsl")
            },
            .TestName = "SectionHierarchyByteWriterTests",
            .ThreadGroupCount{1, 1, 1},
            .TestDataLayout
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