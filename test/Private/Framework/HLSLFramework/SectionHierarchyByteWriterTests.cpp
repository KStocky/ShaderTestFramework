#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - SectionHierarchy - ByteWriter")
{
    ShaderTestFixture::Desc desc{};

    desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
    desc.HLSLVersion = EHLSLVersion::v2021;
    desc.Source = std::move(fs::path("/Tests/SectionHierarchyByteWriterTests.hlsl"));
    desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    desc.TestDataLayout = STF::TestDataBufferLayout(100, 1024, 100, 6400, 100);
    ShaderTestFixture fixture(std::move(desc));

    //fixture.TakeCapture();
    REQUIRE(fixture.RunTest("SectionHierarchyByteWriterTests", 1, 1, 1));
}