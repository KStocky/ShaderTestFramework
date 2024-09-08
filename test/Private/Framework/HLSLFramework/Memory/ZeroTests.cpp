#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Memory - Zero")
{
    using namespace stf;
    auto testName = GENERATE
    (
        "GIVEN_ObjectIsInitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed",
        "GIVEN_ObjectIsUnitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed",
        "GIVEN_NoObjectAlreadyExists_WHEN_Zeroed_THEN_AllMembersOfReturnedObjectAreZeroed"
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