#pragma once

#include "Framework/ShaderTestFixture.h"
#include "D3D12/Shader/ShaderEnums.h"

inline VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    fs::path shaderDir = fs::current_path();
    shaderDir += "/";
    shaderDir += SHADER_SRC;
    shaderDir += "/HLSLFrameworkTests/";

    return VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) };
}

inline ShaderTestFixture::Desc CreateDescForHLSLFrameworkTest(fs::path&& InPath, STF::TestDataBufferLayout InTestDataLayout = {10, 1024})
{
    ShaderTestFixture::Desc desc{};

    desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
    desc.Source = std::move(InPath);
    desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    desc.TestDataLayout = InTestDataLayout;
    return desc;
}