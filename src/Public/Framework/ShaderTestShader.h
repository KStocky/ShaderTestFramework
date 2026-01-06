
#pragma once

#include "Platform.h"

#include "D3D12/CommandEngine.h"
#include "D3D12/CommandList.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/CompiledShaderData.h"
#include "D3D12/Shader/RootSignature.h"
#include "D3D12/Shader/Shader.h"

#include "Framework/ShaderTestCommon.h"
#include "Utility/Expected.h"
#include "Utility/HLSLTypes.h"
#include "Utility/Object.h"
#include "Utility/TransparentStringHash.h"

#include <unordered_map>
#include <span>
#include <vector>

namespace stf
{
    class ShaderTestShader 
        : public Object
    {
    public:
        struct CreationParams
        {
            SharedPtr<Shader> Shader;
        };

        struct TestBindings
        {
            uint3 DispatchConfig{0};
            u32 AllocationBufferIndex = 0;
            u32 TestDataBufferIndex = 0;
            TestDataBufferLayout TestDataLayout{};
        };

        ShaderTestShader(ObjectToken, const CreationParams& InParams);

        ExpectedError<void> StageConstantBufferData(const TestBindings& InTestBindings, const std::span<const ShaderBinding> InBindings);
        void CommitBindings(ScopedCommandContext& InList) const;

        uint3 GetThreadGroupSize() const;

        const RootSignature& GetRootSig() const;

        IDxcBlob* GetCompiledShader() const;

    private:

        SharedPtr<Shader> m_Shader;
    };
}