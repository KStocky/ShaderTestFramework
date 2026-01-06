
#include "D3D12/Shader/Shader.h"

namespace stf
{
    namespace Errors
    {
        Error NoShaderReflectionDataAvailable()
        {
            return Error::FromFragment<"No reflection data generated.Did you compile your shader as a lib ? Libs do not generate reflection data">();
        }
    }

    Shader::Shader(ObjectToken InToken, ShaderToken, const CreationParams& InParams)
        : Object(InToken)
        , m_ShaderData(InParams.ShaderData)
        , m_BindingMap(InParams.BindingMap)
    {
    }

    ExpectedError<SharedPtr<Shader>> Shader::Make(const CompiledShaderData& InShaderData, GPUDevice& InDevice)
    {
        auto reflection = InShaderData.GetReflection();

        if (!reflection)
        {
            return Unexpected{ Errors::NoShaderReflectionDataAvailable() };
        }

        return ShaderBindingMap::Make(*reflection, InDevice)
            .and_then(
                [&](const ShaderBindingMap& InShaderBindingMap) -> ExpectedError<Shader::CreationParams>
                {
                    return Shader::CreationParams
                    {
                        .ShaderData = InShaderData,
                        .BindingMap = InShaderBindingMap
                    };
                })
            .and_then(
                [](const Shader::CreationParams& InCreationParams) -> ExpectedError<SharedPtr<Shader>>
                {
                    return Object::New<Shader>(ShaderToken{}, InCreationParams);
                });
    }

    ExpectedError<void> Shader::StageBindingData(const ShaderBinding& InBinding)
    {
        return m_BindingMap.StageBindingData(InBinding);
    }

    void Shader::CommitBindings(ScopedCommandContext& InContext) const
    {
        m_BindingMap.CommitBindings(InContext);
    }

    uint3 Shader::GetThreadGroupSize() const
    {
        uint3 ret;
        m_ShaderData.GetReflection()->GetThreadGroupSize(&ret.x, &ret.y, &ret.z);

        return ret;
    }

    const RootSignature& Shader::GetRootSig() const
    {
        return m_BindingMap.GetRootSig();
    }

    IDxcBlob* Shader::GetCompiledShader() const
    {
        return m_ShaderData.GetCompiledShader();
    }
}