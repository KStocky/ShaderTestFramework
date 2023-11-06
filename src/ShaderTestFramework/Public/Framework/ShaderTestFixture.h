#pragma once

#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/ShaderCompiler.h"
#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"

#include <vector>

class ShaderTestFixture
{
public:

    struct Desc
    {
        std::vector<VirtualShaderDirectoryMapping> Mappings;
        GPUDevice::CreationParams GPUDeviceParams = {
            .DebugLevel = GPUDevice::EDebugLevel::DebugLayerWithValidation,
            .DeviceType = GPUDevice::EDeviceType::Software
        };
        std::string TestFile;
        std::vector<std::string> CompilationFlags;
    };

    ShaderTestFixture(Desc InParams);

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    GPUDevice m_Device;
    ShaderCompiler m_Compiler;
};
