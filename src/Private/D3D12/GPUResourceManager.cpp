
#include "D3D12/GPUResourceManager.h"

#include "Utility/Math.h"

#include <d3dx12/d3dx12.h>

namespace stf
{
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

    GPUResourceManager::ConstantBufferHandle GPUResourceManager::Acquire(const ConstantBufferDesc InDesc)
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

    GPUResourceManager::ConstantBufferViewHandle GPUResourceManager::CreateCBV(const ConstantBufferHandle InBufferHandle)
    {
        auto descriptor = ThrowIfUnexpected(m_DescriptorManager->Acquire()
            .or_else(
                [&](const DescriptorManager::EErrorType InErrorType) -> DescriptorManager::Expected<DescriptorManager::Descriptor>
                {
                    switch (InErrorType)
                    {
                        case DescriptorManager::EErrorType::AllocatorFull:
                        {
                            auto oldHeap = ThrowIfUnexpected(m_DescriptorManager->Resize(m_DescriptorManager->GetCapacity() * 2));
                            ThrowIfUnexpected(m_HeapReleaseManager.Release(m_HeapReleaseManager.Manage(std::move(oldHeap))));
                            return m_DescriptorManager->Acquire();
                        }
                        default:
                        {
                            return Unexpected{ InErrorType };
                        }
                    }
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

    void GPUResourceManager::UploadData(const std::span<const std::byte>, const ConstantBufferHandle)
    {
    }

    void GPUResourceManager::Release(const ConstantBufferHandle InHandle)
    {
        ThrowIfUnexpected(m_Resources.Release(InHandle.GetHandle()));
    }

    void GPUResourceManager::Release(const ConstantBufferViewHandle InHandle)
    {
        ThrowIfUnexpected(m_Descriptors.Release(InHandle.GetCBVHandle()));
    }
}
