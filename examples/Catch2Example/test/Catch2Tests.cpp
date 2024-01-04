#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

static VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    return VirtualShaderDirectoryMapping{ "/MyTests", fs::current_path() / "shader" };
}

static ShaderTestFixture::Desc CreateDescForHLSLFrameworkTest(fs::path&& InPath, STF::AssertBufferLayout InAssertParams = {10, 1024})
{
    ShaderTestFixture::Desc desc{};

    desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
    desc.HLSLVersion = EHLSLVersion::v2021;
    desc.Source = std::move(InPath);
    desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    desc.AssertInfo = InAssertParams;
    return desc;
}

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("BasicShaderTests")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"ThisTestDoesNotExist", false},
                std::tuple{"ThisTestShouldPass", true},
                std::tuple{"ThisTestShouldFail", false},
            }
        )
    );
    ShaderTestFixture fixture(CreateDescForHLSLFrameworkTest(fs::path("/MyTests/ShaderTest.hlsl")));
    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testName, 1, 1, 1));
        }
        else
        {
            const auto result = fixture.RunTest(testName, 1, 1, 1);
            REQUIRE_FALSE(result);
        }
        
    }
}