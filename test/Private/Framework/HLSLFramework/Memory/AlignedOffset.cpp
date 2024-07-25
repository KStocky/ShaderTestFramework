#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Memory - AlignedOffset")
{
    auto testName = GENERATE
    (
        "GIVEN_IndexIsAlreadyAligned_WHEN_Aligned_THEN_ReturnsSameIndex",
        "GIVEN_IndexIsZero_WHEN_Aligned_THEN_ReturnsZero",
        "GIVEN_IndexIsOneMinusAlignment_WHEN_Aligned_THEN_Alignment",
        "GIVEN_IndexIsNotAligned_WHEN_Aligned_THEN_ReturnsNextMultipleOfAlignment"
    );

    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(
            ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/Memory/AlignedOffsetTests.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}