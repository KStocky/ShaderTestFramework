
#include "D3D12/Shader/ShaderBindingMap.h"
#include "D3D12/Shader/ShaderReflectionUtils.h"

namespace stf
{
    namespace Errors
    {
        Error OnlyConstantBuffersSupportedForBinding()
        {
            return Error::FromFragment<"Only constant buffers are supported for binding.">();
        }

        Error ConstantBufferCantBeInRootConstants(const std::string_view InBufferName)
        {
            return Error::FromFragment<"Constant Buffer: {} can not be stored in root constants">(InBufferName);
        }

        Error RootSignatureDWORDLimitReached()
        {
            return Error::FromFragment<"Limit of 64 uints reached">();
        }

        Error ConstantBufferMustBeBoundToDecriptorTable(const std::string_view InBufferName)
        {
            return Error::FromFragment<"Constant Buffer: {} must be bound in a descriptor table, which is currently unsupported">(InBufferName);
        }

        Error BindingIsSmallerThanBindingData(const std::string_view InBindingName, const u32 InConstantBufferSize, const u64 InBindingDataSize)
        {
            return Error::FromFragment<"Binding is smaller in size than provided data. Binding name: {}, Binding size: {}, Provided binding data size: {}">
                (
                    InBindingName,
                    InConstantBufferSize,
                    InBindingDataSize
                );
        }

        Error BindingDoesNotExist(const std::string_view InBindingName)
        {
            return Error::FromFragment<"No shader binding named {} found. If the binding exists in the shader, are you using it?">(InBindingName);
        }
    }

    ExpectedError<ShaderBindingMap> ShaderBindingMap::Make(ID3D12ShaderReflection& InReflection, GPUDevice& InDevice)
    {
        D3D12_SHADER_DESC shaderDesc{};
        InReflection.GetDesc(&shaderDesc);

        std::vector<CD3DX12_ROOT_PARAMETER1> parameters;
        parameters.reserve(shaderDesc.BoundResources);

        BindingMapType nameToBindingMap;
        StagingBufferMap stagingBuffers;

        u32 totalNumValues = 0;
        for (u32 boundIndex = 0; boundIndex < shaderDesc.BoundResources; ++boundIndex)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
            InReflection.GetResourceBindingDesc(boundIndex, &bindDesc);

            if (bindDesc.Type != D3D_SIT_CBUFFER)
            {
                return Unexpected{ Errors::OnlyConstantBuffersSupportedForBinding() };
            }

            const auto constantBuffer = InReflection.GetConstantBufferByName(bindDesc.Name);
            D3D12_SHADER_BUFFER_DESC bufferDesc{};
            ThrowIfFailed(constantBuffer->GetDesc(&bufferDesc));

            const EBindType bindingType =
                [&]()
                {
                    if (ConstantBufferCanBeBoundToRootConstants(*constantBuffer))
                    {
                        const u32 numValues = bufferDesc.Size / sizeof(u32);
                        totalNumValues += numValues;

                        if (totalNumValues <= 64)
                        {
                            return EBindType::RootConstants;
                        }
                    }

                    if (ConstantBufferCanBeBoundToRootDescriptor(*constantBuffer))
                    {
                        return EBindType::RootDescriptor;
                    }

                    return EBindType::DescriptorTable;
                }();

            if (bindingType == EBindType::DescriptorTable)
            {
                return Unexpected{ Errors::ConstantBufferMustBeBoundToDecriptorTable(bufferDesc.Name) };
            }

            if (bufferDesc.Name != nullptr && std::string_view{ bufferDesc.Name } == std::string_view{ "$Globals" })
            {
                for (u32 globalIndex = 0; globalIndex < bufferDesc.Variables; ++globalIndex)
                {
                    auto var = constantBuffer->GetVariableByIndex(globalIndex);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    nameToBindingMap.emplace(
                        std::string{ varDesc.Name },
                        BindingInfo{
                            .RootParamIndex = static_cast<u32>(parameters.size()),
                            .OffsetIntoBuffer = varDesc.StartOffset,
                            .BindingSize = varDesc.Size,
                            .Type = bindingType
                        });
                }
            }
            else
            {
                nameToBindingMap.emplace(
                    std::string{ bindDesc.Name },
                    BindingInfo{
                        .RootParamIndex = static_cast<u32>(parameters.size()),
                        .OffsetIntoBuffer = 0,
                        .BindingSize = bufferDesc.Size,
                        .Type = bindingType
                    });
            }

            auto& stagingBuffer = stagingBuffers[static_cast<u32>(parameters.size())];
            stagingBuffer.Buffer.resize(bufferDesc.Size);
            stagingBuffer.Type = bindingType;

            auto& parameter = parameters.emplace_back();

            switch (bindingType)
            {
                case EBindType::RootConstants:
                {
                    const u32 numValues = bufferDesc.Size / sizeof(u32);
                    parameter.InitAsConstants(numValues, bindDesc.BindPoint, bindDesc.Space);
                    break;
                }
                case EBindType::RootDescriptor:
                {
                    parameter.InitAsConstantBufferView(bindDesc.BindPoint, bindDesc.Space);
                    break;
                }
                default:
                {
                    std::unreachable();
                }
            }
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc{};
        rootSigDesc.Init_1_1(static_cast<u32>(parameters.size()), parameters.data(), 0u, nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
        );

        return ShaderBindingMap
        {
            InDevice.CreateRootSignature(rootSigDesc),
            std::move(nameToBindingMap),
            std::move(stagingBuffers)
        };
    }

    const RootSignature& ShaderBindingMap::GetRootSig() const
    {
        return *m_RootSignature;
    }

    ExpectedError<void> ShaderBindingMap::StageBindingData(const ShaderBinding& InBinding)
    {
        if (const auto bindingIter = m_NameToBindingInfo.find(InBinding.GetName()); bindingIter != m_NameToBindingInfo.cend())
        {
            const auto bindingData = InBinding.GetBindingData();
            if (bindingIter->second.BindingSize < bindingData.size_bytes())
            {
                return Unexpected{ Errors::BindingIsSmallerThanBindingData(
                    bindingIter->first,
                    bindingIter->second.BindingSize,
                    bindingData.size_bytes()) };
            }

            auto& bindingBuffer = m_RootParamBuffers[bindingIter->second.RootParamIndex];

            ThrowIfFalse(bindingBuffer.Buffer.size() > 0, "Shader binding buffer has a size of zero. It should have been created and initialized when processing the reflection data");

            const u32 offset = bindingIter->second.OffsetIntoBuffer;
            std::memcpy(bindingBuffer.Buffer.data() + offset, bindingData.data(), bindingData.size_bytes());
        }
        else
        {
            return Unexpected{ Errors::BindingDoesNotExist(InBinding.GetName()) };
        }

        return {};
    }

    /*
    void ShaderBindingMap::CommitBindings(ScopedCommandContext& InContext) const
    {
        for (const auto& [rootParamIndex, buffer] : m_RootParamBuffers)
        {
            switch (buffer.Type)
            {
                case EBindType::RootConstants:
                {
                    InContext->SetComputeRoot32BitConstants(rootParamIndex, std::span{ buffer.Buffer }, 0);
                    break;
                }
                case EBindType::RootDescriptor:
                {
                    const auto cbv = InContext.CreateCBV(std::as_bytes( std::span{buffer.Buffer} ));
                    InContext.SetRootDescriptor(rootParamIndex, cbv);
                    break;
                }
                default:
                {
                    std::unreachable();
                }
            }
        }
    }
    */
    ShaderBindingMap::ShaderBindingMap(
        SharedPtr<RootSignature>&& InRootSignature,
        BindingMapType&& InNameToBindingsMap,
        StagingBufferMap&& InStagingBufferMap
    )
        : m_RootSignature(std::move(InRootSignature))
        , m_NameToBindingInfo(std::move(InNameToBindingsMap))
        , m_RootParamBuffers(std::move(InStagingBufferMap))
    {
    }
}