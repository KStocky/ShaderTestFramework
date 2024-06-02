#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - SectionManagement")
{
    auto testName = GENERATE
    (
        "GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce",
        "GIVEN_SingleSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered2Times",
        "GIVEN_TwoSubsections_WHEN_RanUsingWhile_THEN_SectionsEntered4Times",
        "GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered5Times"
    );

    ShaderTestFixture::Desc desc{};

    desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
    desc.Source = fs::path("/Tests/SectionManagement.hlsl");
    desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    desc.TestDataLayout = STF::TestDataBufferLayout(100, 1024);

    ShaderTestFixture fixture(std::move(desc));
    DYNAMIC_SECTION(testName)
    {
        REQUIRE(fixture.RunTest(testName, 1, 1, 1));
    }
}