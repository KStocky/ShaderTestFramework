#pragma once

#include "Framework/ShaderTestFixture.h"
#include "D3D12/Shader/ShaderEnums.h"

inline VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    fs::path shaderDir = fs::current_path();
    shaderDir += "/";
    shaderDir += SHADER_SRC;
    shaderDir += "/Framework/ShaderTestFixtureTests";

    return VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) };
}

inline ShaderTestFixture::Desc CreateDescForHLSLFrameworkTest(fs::path&& InPath, STF::AssertBufferLayout InAssertParams = {10, 1024})
{
    ShaderTestFixture::Desc desc{};

    desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
    desc.HLSLVersion = EHLSLVersion::v2021;
    desc.Source = std::move(InPath);
    desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    desc.AssertInfo = InAssertParams;
    return desc;
}