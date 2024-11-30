#include "D3D12/Shader/CompiledShaderData.h"

namespace stf
{
    CompiledShaderData::CompiledShaderData(ShaderCompilerToken, CreationParams InParams)
        : m_CompiledShader(std::move(InParams.CompiledShader))
        , m_Reflection(std::move(InParams.Reflection))
        , m_Hash(std::move(InParams.Hash))
    {
    }

    IDxcBlob* CompiledShaderData::GetCompiledShader() const
    {
        return m_CompiledShader.Get();
    }

    const std::optional<ShaderHash>& CompiledShaderData::GetShaderHash() const
    {
        return m_Hash;
    }

    ID3D12ShaderReflection* CompiledShaderData::GetReflection() const
    {
        return m_Reflection.Get();
    }
}