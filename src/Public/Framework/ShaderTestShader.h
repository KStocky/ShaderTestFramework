
#pragma once

#include "Platform.h"

#include "D3D12/CommandList.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/CompiledShaderData.h"
#include "D3D12/Shader/RootSignature.h"
#include "D3D12/Shader/ShaderBinding.h"

#include "Framework/HLSLTypes.h"
#include "Framework/ShaderTestCommon.h"
#include "Utility/Expected.h"
#include "Utility/Object.h"
#include "Utility/TransparentStringHash.h"

#include <unordered_map>
#include <span>
#include <vector>

namespace stf
{
    class ShaderTestShader : Object
    {
    public:
        struct CreationParams
        {
            CompiledShaderData ShaderData;
            SharedPtr<GPUDevice> Device;
        };

        ShaderTestShader(CreationParams InParams);

        Expected<void, ErrorTypeAndDescription> Init();
        Expected<void, ErrorTypeAndDescription> BindConstantBufferData(const std::span<const ShaderBinding> InBindings);
        void SetConstantBufferData(CommandList& InList) const;

        uint3 GetThreadGroupSize() const;

        RootSignature* GetRootSig() const;

        IDxcBlob* GetCompiledShader() const;

    private:

        struct BindingInfo
        {
            u32 RootParamIndex = 0;
            u32 OffsetIntoBuffer = 0;
            u32 BindingSize = 0;
        };

        CompiledShaderData m_ShaderData;
        SharedPtr<GPUDevice> m_Device;
        SharedPtr<RootSignature> m_RootSignature;

        std::unordered_map<std::string, BindingInfo, TransparentStringHash, std::equal_to<>> m_NameToBindingInfo;
        std::unordered_map<u32, std::vector<u32>> m_RootParamBuffers;
    };
}