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

        template<typename ThisType>
        decltype(auto) GetCompiledShader(this ThisType&& InThis)
        {
            return std::forward<ThisType>(InThis).m_CompiledShader.Get();
        }

        template<typename ThisType>
        decltype(auto) GetShaderHash(this ThisType&& InThis)
        {
            return std::forward<ThisType>(InThis).m_Hash;
        }

        template<typename ThisType>
        decltype(auto) GetReflection(this ThisType&& InThis)
        {
            return std::forward<ThisType>(InThis).m_Reflection;
        }

    private:

        ComPtr<IDxcBlob> m_CompiledShader = nullptr;
        ComPtr<ID3D12ShaderReflection> m_Reflection = nullptr;
        std::optional<ShaderHash> m_Hash;
    };
}