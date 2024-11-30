#pragma once
#include "D3D12/Shader/ShaderHash.h"
#include "Utility/Pointer.h"

#include <optional>
#include <string>

#include <d3d12shader.h>
#include <dxcapi.h>

namespace stf
{
    class ShaderCompilerToken
    {
        friend class ShaderCompiler;
        ShaderCompilerToken() = default;
    };

    class CompiledShaderData
    {
    public:
        struct CreationParams
        {
            ComPtr<IDxcBlob> CompiledShader = nullptr;
            ComPtr<ID3D12ShaderReflection> Reflection = nullptr;
            std::optional<ShaderHash> Hash = {};
        };

        CompiledShaderData() = default;
        CompiledShaderData(ShaderCompilerToken, CreationParams InParams);

        IDxcBlob* GetCompiledShader() const;

        const std::optional<ShaderHash>& GetShaderHash() const;

        ID3D12ShaderReflection* GetReflection() const;

    private:

        ComPtr<IDxcBlob> m_CompiledShader = nullptr;
        ComPtr<ID3D12ShaderReflection> m_Reflection = nullptr;
        std::optional<ShaderHash> m_Hash;
    };
}