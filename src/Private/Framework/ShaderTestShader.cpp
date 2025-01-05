
#include "Framework/ShaderTestShader.h"
#include "D3D12/Shader/ShaderReflectionUtils.h"

namespace stf
{
    ShaderTestShader::ShaderTestShader(ObjectToken InToken, CreationParams InParams)
        : Object(InToken)
        , m_ShaderData(std::move(InParams.ShaderData))
        , m_Device(std::move(InParams.Device))
        , m_RootSignature()
        , m_NameToBindingInfo()
        , m_RootParamBuffers()
    {
    }

    Expected<void, ErrorTypeAndDescription> ShaderTestShader::BindConstantBufferData(const std::span<const ShaderBinding> InBindings)
    {
        return 
            Init()
            .and_then(
                [this, InBindings]() -> Expected<void, ErrorTypeAndDescription>
                {
                    for (const auto& binding : InBindings)
                    {
                        const auto bindingInfo = m_NameToBindingInfo.find(binding.GetName());
                        if (bindingInfo == m_NameToBindingInfo.cend())
                        {
                            if (binding.GetName().contains("stf::detail::"))
                            {
                                continue;
                            }
                            else
                            {
                                return Unexpected(ErrorTypeAndDescription
                                    {
                                        .Type = ETestRunErrorType::Binding,
                                        .Error = std::format("No shader binding named {} found in test shader", binding.GetName())
                                    });
                            }
                        }

                        auto& bindingBuffer = m_RootParamBuffers[bindingInfo->second.RootParamIndex];

                        ThrowIfFalse(bindingBuffer.size() > 0, "Shader binding buffer has a size of zero. It should have been created and initialized when processing the reflection data");

                        const auto bindingData = binding.GetBindingData();
                        if (bindingData.size_bytes() > bindingInfo->second.BindingSize)
                        {
                            return
                                Unexpected(ErrorTypeAndDescription
                                    {
                                        .Type = ETestRunErrorType::Binding,
                                        .Error = std::format("Shader binding {0} provided is larger than the expected binding size", binding.GetName())
                                    });
                        }

                        const u32 uintIndex = bindingInfo->second.OffsetIntoBuffer / sizeof(u32);
                        std::memcpy(bindingBuffer.data() + uintIndex, bindingData.data(), bindingData.size_bytes());
                    }

                    return {};
                });
    }

    void ShaderTestShader::SetConstantBufferData(CommandList& InList) const
    {
        for (const auto& [paramIndex, buffer] : m_RootParamBuffers)
        {
            InList.SetComputeRoot32BitConstants(paramIndex, std::span{ buffer }, 0);
        }
    }

    uint3 ShaderTestShader::GetThreadGroupSize() const
    {
        uint3 ret;
        m_ShaderData.GetReflection()->GetThreadGroupSize(&ret.x, &ret.y, &ret.z);

        return ret;
    }

    RootSignature* ShaderTestShader::GetRootSig() const
    {
        return m_RootSignature.get();
    }

    IDxcBlob* ShaderTestShader::GetCompiledShader() const
    {
        return m_ShaderData.GetCompiledShader();
    }

    Expected<void, ErrorTypeAndDescription> ShaderTestShader::Init()
    {
        if (m_RootSignature)
        {
            return {};
        }

        const auto refl = m_ShaderData.GetReflection();

        if (!refl)
        {
            return std::unexpected(ErrorTypeAndDescription
                {
                    .Type = ETestRunErrorType::RootSignatureGeneration,
                    .Error = "No reflection data generated. Did you compile your shader as a lib? Libs do not generate reflection data"
                });
        }

        D3D12_SHADER_DESC shaderDesc{};
        refl->GetDesc(&shaderDesc);

        std::vector<CD3DX12_ROOT_PARAMETER1> parameters;
        parameters.reserve(shaderDesc.BoundResources);

        u32 totalNumValues = 0;
        for (u32 boundIndex = 0; boundIndex < shaderDesc.BoundResources; ++boundIndex)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
            refl->GetResourceBindingDesc(boundIndex, &bindDesc);

            if (bindDesc.Type != D3D_SIT_CBUFFER)
            {
                return std::unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::RootSignatureGeneration,
                        .Error = "Only constant buffers are supported for binding."
                    });
            }

            const auto constantBuffer = refl->GetConstantBufferByName(bindDesc.Name);
            D3D12_SHADER_BUFFER_DESC bufferDesc{};
            ThrowIfFailed(constantBuffer->GetDesc(&bufferDesc));

            if (!ConstantBufferCanBeBoundToRootConstants(*constantBuffer))
            {
                return std::unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::RootSignatureGeneration,
                        .Error = std::format("Constant Buffer: {} can not be stored in root constants", bufferDesc.Name)
                    });
            }

            const u32 numValues = bufferDesc.Size / sizeof(u32);
            totalNumValues += numValues;

            if (totalNumValues > 64)
            {
                return std::unexpected(ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::RootSignatureGeneration,
                        .Error = "Limit of 64 uints reached"
                    });
            }

            if (bufferDesc.Name != nullptr && std::string_view{ bufferDesc.Name } == std::string_view{ "$Globals" })
            {
                for (u32 globalIndex = 0; globalIndex < bufferDesc.Variables; ++globalIndex)
                {
                    auto var = constantBuffer->GetVariableByIndex(globalIndex);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    m_NameToBindingInfo.emplace(
                        std::string{ varDesc.Name },
                        BindingInfo{
                            .RootParamIndex = static_cast<u32>(parameters.size()),
                            .OffsetIntoBuffer = varDesc.StartOffset,
                            .BindingSize = varDesc.Size
                        });
                }
            }
            else
            {
                m_NameToBindingInfo.emplace(
                    std::string{ bindDesc.Name },
                    BindingInfo{
                        .RootParamIndex = static_cast<u32>(parameters.size()),
                        .OffsetIntoBuffer = 0,
                        .BindingSize = bufferDesc.Size
                    });
            }

            m_RootParamBuffers[static_cast<u32>(parameters.size())].resize(bufferDesc.Size / sizeof(u32));

            auto& parameter = parameters.emplace_back();
            parameter.InitAsConstants(numValues, bindDesc.BindPoint, bindDesc.Space);
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig{};
        rootSig.Init_1_1(static_cast<u32>(parameters.size()), parameters.data(), 0u, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
        );

        m_RootSignature = m_Device->CreateRootSignature(rootSig);

        return {};
    }
}