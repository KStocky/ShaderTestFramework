#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - Memory - Zero")
{
    auto testName = GENERATE
    (
        "GIVEN_ObjectIsInitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed",
        "GIVEN_ObjectIsUnitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed",
        "GIVEN_NoObjectAlreadyExists_WHEN_Zeroed_THEN_AllMembersOfReturnedObjectAreZeroed"
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
                    .Source = fs::path("/Tests/Memory/ZeroTests.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1}
            }
        ));
    }
}