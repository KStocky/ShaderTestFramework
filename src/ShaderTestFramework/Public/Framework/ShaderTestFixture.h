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
        ShaderCodeSource Source;
        std::vector<std::wstring> CompilationFlags;
        D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_6;
    };

    class Results
    {
    public:

        Results(std::vector<std::string> InErrors);

        template<typename ThisType>
        operator bool(this ThisType&& InThis)
        {
            return InThis.m_Errors.empty();
        }

        friend std::ostream& operator<<(std::ostream& InOs, const Results& In);

    private:

        std::vector<std::string> m_Errors;
    };

    ShaderTestFixture(Desc InParams);

    Results RunTest(std::string InName, u32 InX, u32 InY, u32 InZ);

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    GPUDevice m_Device;
    ShaderCompiler m_Compiler;
    ShaderCodeSource m_Source;
    std::vector<std::wstring> m_CompilationFlags;
    D3D_SHADER_MODEL m_ShaderModel;
    bool m_IsWarp = false;
};
