
#include "Framework/ShaderTestShader.h"
#include "D3D12/Shader/ShaderReflectionUtils.h"

namespace stf
{
    ShaderTestShader::ShaderTestShader(ObjectToken InToken, const CreationParams& InParams)
        : Object(InToken)
        , m_Shader(InParams.Shader)
    {
    }

    ExpectedError<void> ShaderTestShader::StageConstantBufferData(const TestBindings& InTestBindings, const std::span<const ShaderBinding> InBindings)
    {
        std::ignore = m_Shader->StageBindingData(ShaderBinding{ "stf::detail::DispatchDimensions", InTestBindings.DispatchConfig * GetThreadGroupSize() });
        std::ignore = m_Shader->StageBindingData(ShaderBinding{ "stf::detail::AllocationBufferIndex", InTestBindings.AllocationBufferIndex });
        std::ignore = m_Shader->StageBindingData(ShaderBinding{ "stf::detail::TestDataBufferIndex", InTestBindings.TestDataBufferIndex });
        std::ignore = m_Shader->StageBindingData(ShaderBinding{ "stf::detail::Asserts", InTestBindings.TestDataLayout.GetAssertSection() });
        std::ignore = m_Shader->StageBindingData(ShaderBinding{ "stf::detail::Strings", InTestBindings.TestDataLayout.GetStringSection() });
        std::ignore = m_Shader->StageBindingData(ShaderBinding{ "stf::detail::Sections", InTestBindings.TestDataLayout.GetSectionInfoSection() });

        for (const auto& binding : InBindings)
        {
            if (auto result = m_Shader->StageBindingData(binding); !result)
            {
                return result;
            }
        }

        return {};
    }

    void ShaderTestShader::CommitBindings(ScopedCommandContext& InCommandContext) const
    {
        m_Shader->CommitBindings(InCommandContext);
    }

    uint3 ShaderTestShader::GetThreadGroupSize() const
    {
        return m_Shader->GetThreadGroupSize();
    }

    const RootSignature& ShaderTestShader::GetRootSig() const
    {
        return m_Shader->GetRootSig();
    }

    IDxcBlob* ShaderTestShader::GetCompiledShader() const
    {
        return m_Shader->GetCompiledShader();
    }
}