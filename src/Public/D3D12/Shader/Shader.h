
#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/Shader/CompiledShaderData.h"
#include "D3D12/Shader/RootSignature.h"
#include "D3D12/Shader/ShaderBinding.h"
#include "D3D12/Shader/ShaderBindingMap.h"
#include "Utility/Error.h"
#include "Utility/HLSLTypes.h"
#include "Utility/Object.h"

namespace stf
{
    namespace Errors
    {
        Error NoShaderReflectionDataAvailable();
    }

    class ShaderToken
    {
        ShaderToken() = default;
        friend class Shader;
    };

    class Shader
        : public Object
    {
    public:
        struct CreationParams
        {
            CompiledShaderData ShaderData;
            ShaderBindingMap BindingMap;
        };

        Shader(ObjectToken, ShaderToken, const CreationParams& InParams);

        static ExpectedError<SharedPtr<Shader>> Make(const CompiledShaderData& InShaderData, GPUDevice& InDevice);

        ExpectedError<void> StageBindingData(const ShaderBinding& InBindings);
        void CommitBindings(ScopedCommandContext& InContext) const;

        uint3 GetThreadGroupSize() const;

        const RootSignature& GetRootSig() const;

        IDxcBlob* GetCompiledShader() const;

    private:

        CompiledShaderData m_ShaderData;
        ShaderBindingMap m_BindingMap;
    };
}