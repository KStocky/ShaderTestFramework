
#include "D3D12/GPUResourceManager.h"

#include "Utility/Math.h"

#include <d3dx12/d3dx12.h>

namespace stf
{
    namespace Errors::GPUResourceManager
    {
        ErrorFragment ReadbackHasNotBeenCompleted(const std::string_view InSourceName)
        {
            return ErrorFragment::Make<"Readback of {} has not completed yet.">(InSourceName);
        }
    }

    GPUResourceManager::GPUResourceManager(ObjectToken InToken, const CreationParams& InParams)
        : Object(InToken)
        , m_Device(InParams.Device)
        , m_Queue(InParams.Queue)
        , m_DescriptorManager(
            Object::New<DescriptorManager>
            (
                DescriptorManager::CreationParams
                {
                    .Device = InParams.Device,
                    .InitialSize = 1000
                }
            )
        )
        , m_Resources{ ResourceManager::CreationParams{.Queue = InParams.Queue} }
        , m_Descriptors{ DescriptorFreeList::CreationParams{ .Queue = InParams.Queue } }
        , m_HeapReleaseManager{ DescriptorHeapReleaseManager::CreationParams { .Queue = InParams.Queue } }
    {
    }

    GPUResourceManager::ConstantBufferHandle::ConstantBufferHandle(Private, const ResourceHandle InHandle)
        : m_Handle(InHandle)
    {
    }

    GPUResourceManager::ResourceHandle GPUResourceManager::ConstantBufferHandle::GetHandle() const
    {
        return m_Handle;
    }

    GPUResourceManager::ConstantBufferViewHandle::ConstantBufferViewHandle(Private, const ResourceHandle InBufferHandle, const DescriptorOpaqueHandle InCBVHandle)
        : m_BufferHandle(InBufferHandle)
        , m_CBVHandle(InCBVHandle)
    {
    }

    GPUResourceManager::ResourceHandle GPUResourceManager::ConstantBufferViewHandle::GetBufferHandle() const
    {
        return m_BufferHandle;
    }

    GPUResourceManager::DescriptorOpaqueHandle GPUResourceManager::ConstantBufferViewHandle::GetCBVHandle() const
    {
        return m_CBVHandle;
    }

    GPUResourceManager::BufferHandle::BufferHandle(Private, const ResourceHandle InHandle)
        : m_Handle(InHandle)
    {
    }

    GPUResourceManager::ResourceHandle GPUResourceManager::BufferHandle::GetHandle() const
    {
        return m_Handle;
    }

    GPUResourceManager::BufferUAVHandle::BufferUAVHandle(Private, const ResourceHandle InBufferHandle, const DescriptorOpaqueHandle InUAVHandle)
        : m_BufferHandle(InBufferHandle)
        , m_UAVHandle(InUAVHandle)
    {
    }

    GPUResourceManager::ResourceHandle GPUResourceManager::BufferUAVHandle::GetBufferHandle() const
    {
        return m_BufferHandle;
    }

    GPUResourceManager::DescriptorOpaqueHandle GPUResourceManager::BufferUAVHandle::GetUAVHandle() const
    {
        return m_UAVHandle;
    }

    GPUResourceManager::ReadbackBufferHandle::ReadbackBufferHandle(Private, const ResourceHandle InReadbackHandle, const ResourceHandle InSourceHandle)
        : m_ReadbackHandle(InReadbackHandle)
        , m_SourceHandle(InSourceHandle)
    {
    }

    GPUResourceManager::ResourceHandle GPUResourceManager::ReadbackBufferHandle::GetReadbackHandle() const
    {
        return m_ReadbackHandle;
    }

    GPUResourceManager::ResourceHandle GPUResourceManager::ReadbackBufferHandle::GetSourceHandle() const
    {
        return m_SourceHandle;
    }

    GPUResourceManager::ReadbackResultHandle::ReadbackResultHandle(Private, const InFlightReadbackHandle InHandle)
        : m_Handle(InHandle)
    {
    }

    GPUResourceManager::InFlightReadbackHandle GPUResourceManager::ReadbackResultHandle::GetReadbackHandle() const
    {
        return m_Handle;
    }

    GPUResourceManager::BufferHandle GPUResourceManager::Acquire(const BufferDesc& InDesc)
    {
        const auto bufferHandle = m_Resources.Manage(
            m_Device->CreateCommittedResource(
                GPUDevice::CommittedResourceDesc
                {
                    .HeapProps = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT },
                    .ResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InDesc.RequestedSize, InDesc.Flags),
                    .Name = InDesc.Name
                }
            )
        );

        return BufferHandle{ Private{}, bufferHandle };
    }

    GPUResourceManager::ConstantBufferHandle GPUResourceManager::Acquire(const ConstantBufferDesc& InDesc)
    {
        const u64 bufferSize = AlignedOffset(InDesc.RequestedSize, 256ull);
        const auto bufferHandle = m_Resources.Manage(
            m_Device->CreateCommittedResource(
                GPUDevice::CommittedResourceDesc
                {
                    .HeapProps = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_UPLOAD },
                    .ResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(bufferSize),
                    .Name = InDesc.Name
                }
            )
        );

        return ConstantBufferHandle{ Private{}, bufferHandle };
    }

    ExpectedError<GPUResourceManager::ReadbackBufferHandle> GPUResourceManager::Acquire(const ReadbackBufferDesc& InDesc)
    {
        return m_Resources.Get(InDesc.Source.GetHandle())
            .and_then(
                [&](const SharedPtr<GPUResource>& InSourceBuffer) -> ExpectedError<GPUResourceManager::ReadbackBufferHandle>
                {
                    const auto readbackHandle = m_Resources.Manage(
                        m_Device->CreateCommittedResource(
                            GPUDevice::CommittedResourceDesc
                            {
                                .HeapProps = CD3DX12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_READBACK },
                                .ResourceDesc = CD3DX12_RESOURCE_DESC1::Buffer(InSourceBuffer->GetDesc().Width),
                                .Name = std::format("Readback buffer for -> {}", InSourceBuffer->GetName())
                            }
                        )
                    );

                    return ReadbackBufferHandle{ Private{}, readbackHandle, InDesc.Source.GetHandle()};
                }
            );
    }

    GPUResourceManager::BufferUAVHandle GPUResourceManager::CreateUAV(const BufferHandle InHandle, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
    {
        auto descriptor = ThrowIfUnexpected(m_DescriptorManager->Acquire()
            .or_else(
                [&](const Error& InErrorType) -> ExpectedError<DescriptorManager::Descriptor>
                {
                    if (InErrorType.HasFragment(Errors::DescriptorManagerIsFull()))
                    {
                        auto oldHeap = ThrowIfUnexpected(m_DescriptorManager->Resize(m_DescriptorManager->GetCapacity() * 2));
                        ThrowIfUnexpected(m_HeapReleaseManager.Release(m_HeapReleaseManager.Manage(std::move(oldHeap))));
                        return m_DescriptorManager->Acquire();
                    }

                    return Unexpected{ InErrorType };
                }
            ));

        ThrowIfUnexpected(m_Resources.Get(InHandle.GetHandle())
            .and_then
            (
                [&](const SharedPtr<GPUResource>& InResource) -> ExpectedError<void>
                {
                    m_Device->CreateUnorderedAccessView(*InResource, InDesc, ThrowIfUnexpected(descriptor.Resolve()));
                    return {};
                }
            ));

        const auto managedHandle = m_Descriptors.Manage(std::move(descriptor));

        return BufferUAVHandle{ Private{}, InHandle.GetHandle(), managedHandle };
    }

    GPUResourceManager::ConstantBufferViewHandle GPUResourceManager::CreateCBV(const ConstantBufferHandle InBufferHandle)
    {
        auto descriptor = ThrowIfUnexpected(m_DescriptorManager->Acquire()
            .or_else(
                [&](const Error& InErrorType) -> ExpectedError<DescriptorManager::Descriptor>
                {
                    if (InErrorType.HasFragment(Errors::DescriptorManagerIsFull()))
                    {
                        auto oldHeap = ThrowIfUnexpected(m_DescriptorManager->Resize(m_DescriptorManager->GetCapacity() * 2));
                        ThrowIfUnexpected(m_HeapReleaseManager.Release(m_HeapReleaseManager.Manage(std::move(oldHeap))));
                        return m_DescriptorManager->Acquire();
                    }

                    return Unexpected{ InErrorType };
                }
            ));

        ThrowIfUnexpected(m_Resources.Get(InBufferHandle.GetHandle())
            .and_then
            (
                [&](const SharedPtr<GPUResource>& InResource) -> ExpectedError<void>
                {
                    m_Device->CreateConstantBufferView(*InResource, ThrowIfUnexpected(descriptor.Resolve()));
                    return {};
                }
            ));

        const auto managedHandle = m_Descriptors.Manage(std::move(descriptor));

        return ConstantBufferViewHandle{ Private{}, InBufferHandle.GetHandle(), managedHandle};
    }

    ExpectedError<u32> GPUResourceManager::GetDescriptorIndex(const DescriptorOpaqueHandle InHandle) const
    {
        return m_Descriptors.Get(InHandle)
            .and_then(
                [&](const DescriptorManager::Descriptor& InDescriptor)
                {
                    return InDescriptor.Resolve();
                }
            )
            .and_then(
                [](const DescriptorHandle InHandle) -> ExpectedError<u32>
                {
                    return InHandle.GetHeapIndex();
                }
            );
    }

    ExpectedError<void> GPUResourceManager::UploadData(const std::span<const std::byte> InData, const ConstantBufferHandle InHandle)
    {
        return m_Resources.Get(InHandle.GetHandle())
            .and_then
            (
                [&](const SharedPtr<GPUResource>& InResource) -> ExpectedError<void>
                {
                    const auto mappedResource = InResource->Map();
                    const auto mappedData = mappedResource.Get();

                    if (InData.size_bytes() > mappedData.size_bytes())
                    {
                        return Unexpected{
                            Error
                            {
                                ErrorFragment::Make<"Provided data is too large for resource. Provided data: {} bytes, Resource Size {} bytes">(
                                    InData.size_bytes(),
                                    mappedData.size_bytes())
                            }
                        };
                    }

                    std::memcpy(mappedData.data(), InData.data(), InData.size_bytes());
                    return {};
                }
            );
    }

    ExpectedError<void> GPUResourceManager::Release(const BufferHandle InHandle)
    {
        return m_Resources.Release(InHandle.GetHandle());
    }

    ExpectedError<void> GPUResourceManager::Release(const ConstantBufferHandle InHandle)
    {
        return m_Resources.Release(InHandle.GetHandle());
    }

    ExpectedError<void> GPUResourceManager::Release(const BufferUAVHandle InHandle)
    {
        return m_Descriptors.Release(InHandle.GetUAVHandle());
    }

    ExpectedError<void> GPUResourceManager::Release(const ConstantBufferViewHandle InHandle)
    {
        return m_Descriptors.Release(InHandle.GetCBVHandle());
    }

    ExpectedError<GPUResourceManager::ReadbackResultHandle> GPUResourceManager::QueueReadback(CommandList& InCommandList, const ReadbackBufferHandle InHandle)
    {
        return m_Resources.Get(InHandle.GetReadbackHandle())
            .and_then(
                [&](const SharedPtr<GPUResource>& InReadback)
                {
                    return m_Resources.Get(InHandle.GetSourceHandle())
                        .and_then(
                            [&](const SharedPtr<GPUResource>& InSource) -> ExpectedError<ReadbackResultHandle>
                            {
                                InCommandList.CopyBufferResource(*InReadback, *InSource);

                                const auto handle = m_Readbacks.Manage(
                                    InFlightReadback
                                    {
                                        .Handle = InHandle,
                                        .FencePoint = m_Queue->Signal(),
                                        .SourceBufferName = InSource->GetName()
                                    }
                                );

                                return ReadbackResultHandle{ Private{}, handle };
                            }
                        );
                }
            );
    }

    ExpectedError<void> GPUResourceManager::SetRootDescriptor(CommandList& InList, const u32 InRootParamIndex, const ConstantBufferViewHandle InHandle)
    {
        return m_Resources.Get(InHandle.GetBufferHandle())
            .and_then
            (
                [&](const SharedPtr<GPUResource>& InResource) -> ExpectedError<void>
                {
                    InList.SetComputeRootConstantBufferView(InRootParamIndex, *InResource);
                    return {};
                }
            );
    }

    ExpectedError<void> GPUResourceManager::SetUAV(CommandList& InCommandList, const BufferUAVHandle InHandle)
    {
        return m_Resources.Get(InHandle.GetBufferHandle())
            .and_then
            (
                [&](const SharedPtr<GPUResource>& InResource) -> ExpectedError<void>
                {
                    InCommandList.SetBufferUAV(*InResource);
                    return {};
                }
            );
    }

    void GPUResourceManager::SetDescriptorHeap(CommandList& InCommandList)
    {
        m_DescriptorManager->SetDescriptorHeap(InCommandList);
    }
}
