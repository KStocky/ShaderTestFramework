#include "D3D12/CommandEngine.h"

namespace stf
{

    ScopedGPUResourceManager::ScopedGPUResourceManager(const SharedPtr<GPUResourceManager>& InResourceManager)
        : m_ResourceManager(InResourceManager)
    {
    }

    ScopedGPUResourceManager::~ScopedGPUResourceManager() noexcept
    {
        for (const auto& cb : m_ConstantBuffers)
        {
            ThrowIfUnexpected(m_ResourceManager->Release(cb));
        }

        for (const auto& cbv : m_CBVs)
        {
            ThrowIfUnexpected(m_ResourceManager->Release(cbv));
        }

        for (const auto& buffer : m_Buffers)
        {
            ThrowIfUnexpected(m_ResourceManager->Release(buffer));
        }

        for (const auto& bufferUAV : m_BufferUAVs)
        {
            ThrowIfUnexpected(m_ResourceManager->Release(bufferUAV));
        }
    }

    GPUResourceManager::ConstantBufferViewHandle ScopedGPUResourceManager::CreateCBV(const std::span<const std::byte> InData)
    {
        const auto buffer = m_ResourceManager->Acquire(GPUResourceManager::ConstantBufferDesc{ .RequestedSize = static_cast<u32>(InData.size_bytes()) });
        const auto cbv = m_ResourceManager->CreateCBV(buffer);

        ThrowIfUnexpected(m_ResourceManager->UploadData(InData, buffer));

        m_ConstantBuffers.push_back(buffer);
        m_CBVs.push_back(cbv);

        return cbv;
    }

    GPUResourceManager::BufferHandle ScopedGPUResourceManager::CreateBuffer(const GPUResourceManager::BufferDesc& InBufferDesc)
    {
        const auto handle = m_ResourceManager->Acquire(InBufferDesc);
        m_Buffers.push_back(handle);
        return handle;
    }

    GPUResourceManager::BufferUAVHandle ScopedGPUResourceManager::CreateUAV(const GPUResourceManager::BufferHandle& InBufferHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
    {
        const auto handle = m_ResourceManager->CreateUAV(InBufferHandle, InDesc);
        m_BufferUAVs.push_back(handle);
        return handle;
    }

    ExpectedError<GPUResourceManager::ReadbackResultHandle> ScopedGPUResourceManager::QueueReadback(CommandList& InList, const GPUResourceManager::BufferHandle InBufferHandle)
    {
        return m_ResourceManager->Acquire(
            GPUResourceManager::ReadbackBufferDesc
            {
                .Source = InBufferHandle
            })
            .and_then(
                [&](const GPUResourceManager::ReadbackBufferHandle InReadbackHandle)
                {
                    return m_ResourceManager->QueueReadback(InList, InReadbackHandle);
                }
            );
    }

    void ScopedGPUResourceManager::SetUAV(CommandList& InList, const GPUResourceManager::BufferUAVHandle InHandle)
    {
        ThrowIfUnexpected(m_ResourceManager->SetUAV(InList, InHandle));
    }

    void ScopedGPUResourceManager::SetRootDescriptor(CommandList& InList, const u32 InRootParamIndex, const GPUResourceManager::ConstantBufferViewHandle InHandle)
    {
        ThrowIfUnexpected(m_ResourceManager->SetRootDescriptor(InList, InRootParamIndex, InHandle));
    }

    void ScopedGPUResourceManager::SetDescriptorHeap(CommandList& InList)
    {
        m_ResourceManager->SetDescriptorHeap(InList);
    }

    ExpectedError<u32> ScopedGPUResourceManager::GetDescriptorIndex(const GPUResourceManager::DescriptorOpaqueHandle InHandle) const
    {
        return m_ResourceManager->GetDescriptorIndex(InHandle);
    }

    ScopedCommandShader::ScopedCommandShader(const SharedPtr<Shader>& InShader)
        : m_Shader(InShader)
    {
    }

    ExpectedError<void> ScopedCommandShader::StageBindingData(const ShaderBinding& InBinding)
    {
        return m_Shader->StageBindingData(InBinding);
    }

    void ScopedCommandShader::StageBindlessResource(ScopedCommandContext& InContext, std::string InName, const GPUResourceManager::BufferUAVHandle InHandle)
    {
        InContext.StageBindlessResource(CommandShaderToken{}, std::move(InName), InHandle);
    }

    ScopedCommandContext::ScopedCommandContext(CommandEngineToken,
        const SharedPtr<CommandList>& InList,
        const SharedPtr<GPUResourceManager>& InResourceManager
    )
        : m_List(InList)
        , m_ResourceManager(MakeUnique<ScopedGPUResourceManager>(InResourceManager))
    {
    }

    CommandList* ScopedCommandContext::operator->() const
    {
        return GetList();
    }

    CommandList& ScopedCommandContext::operator*() const
    {
        return *GetList();
    }

    CommandList* ScopedCommandContext::GetList() const
    {
        return m_List.get();
    }

    GPUResourceManager::BufferHandle ScopedCommandContext::CreateBuffer(const GPUResourceManager::BufferDesc& InDesc)
    {
        return m_ResourceManager->CreateBuffer(InDesc);
    }

    GPUResourceManager::BufferUAVHandle ScopedCommandContext::CreateUAV(const GPUResourceManager::BufferHandle& InBufferHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
    {
        return m_ResourceManager->CreateUAV(InBufferHandle, InDesc);
    }

    ExpectedError<GPUResourceManager::ReadbackResultHandle> ScopedCommandContext::QueueReadback(const GPUResourceManager::BufferHandle InBufferHandle)
    {
        return m_ResourceManager->QueueReadback(*m_List, InBufferHandle);
    }

    void ScopedCommandContext::SetUAV(const GPUResourceManager::BufferUAVHandle InHandle)
    {
        m_ResourceManager->SetUAV(*m_List, InHandle);
    }

    GPUResourceManager::ConstantBufferViewHandle ScopedCommandContext::CreateCBV(const std::span<const std::byte> InData)
    {
        return m_ResourceManager->CreateCBV(InData);
    }

    void ScopedCommandContext::SetRootDescriptor(const u32 InRootParamIndex, const GPUResourceManager::ConstantBufferViewHandle InHandle)
    {
        m_ResourceManager->SetRootDescriptor(*m_List, InRootParamIndex, InHandle);
    }

    void ScopedCommandContext::StageBindlessResource(CommandShaderToken, std::string InBindingName, const GPUResourceManager::BufferUAVHandle InHandle)
    {
        SetUAV(InHandle);

        m_BindlessResourcesToResolve.emplace_back(
            StagedBindlessResource
            {
                .Name = std::move(InBindingName),
                .Descriptor = InHandle.GetUAVHandle()
            });
    }

    ExpectedError<void> ScopedCommandContext::Dispatch(const uint3 InDispatchConfig)
    {
        //m_ResourceManager->SetDescriptorHeap(*m_List);
        return ResolveAndStageBindlessResources()
            .and_then(
                [&]() -> ExpectedError<void>
                {
                    //m_BoundShader->ForEachStagingBuffer(
                    //    [&](const u32 InRootParamIndex, const ShaderBindingMap::StagingInfo& InStagingInfo)
                    //    {
                    //        switch (InStagingInfo.Type)
                    //        {
                    //            case ShaderBindingMap::EBindType::RootConstants:
                    //            {
                    //                m_List->SetComputeRoot32BitConstants(InRootParamIndex, std::span{ InStagingInfo.Buffer }, 0);
                    //                break;
                    //            }
                    //            case ShaderBindingMap::EBindType::RootDescriptor:
                    //            {
                    //                const auto cbv = CreateCBV(std::as_bytes(std::span{ InStagingInfo.Buffer }));
                    //                SetRootDescriptor(InRootParamIndex, cbv);
                    //                break;
                    //            }
                    //            default:
                    //            {
                    //                std::unreachable();
                    //            }
                    //        }
                    //    }
                    //);

                    m_List->Dispatch(InDispatchConfig.x, InDispatchConfig.y, InDispatchConfig.z);
                    return {};
                }
            );
    }

    ExpectedError<void> ScopedCommandContext::ResolveAndStageBindlessResources()
    {
        if (!m_BindlessResourcesToResolve.empty() && !m_BoundShader)
        {
            return Unexpected{ Error::FromFragment<"THIS SHOULD NOT HAPPEN: There are bindless resources to resolve, with no bound shader">()};
        }

        for (const auto& [bindingName, descriptor] : m_BindlessResourcesToResolve)
        {
            const auto stageResult = m_ResourceManager->GetDescriptorIndex(descriptor)
                .and_then(
                    [&](const u32 InIndex)
                    {
                        return m_BoundShader->StageBindingData(ShaderBinding{ bindingName, InIndex });
                    }
                );

            if (!stageResult)
            {
                return stageResult;
            }
        }

        return {};
    }

    CommandEngine::CommandEngine(ObjectToken InToken, const CreationParams& InParams)
        : Object(InToken)
        , m_Device(InParams.Device)
        , m_Queue(InParams.Device->CreateCommandQueue(
            {
                .Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
                .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
                .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
                .NodeMask = 0
            },
            "Command Engine Direct Queue"))
        , m_List(InParams.Device->CreateCommandList(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            "Command Engine Direct List"
        ))
        , m_ResourceManager(
            Object::New<GPUResourceManager>(
                GPUResourceManager::CreationParams
                {
                    .Device = InParams.Device,
                    .Queue = m_Queue
                }
            )
        )
        , m_Allocators()
    {
    }

    void CommandEngine::Flush()
    {
        m_Queue->FlushQueue();
    }
}
