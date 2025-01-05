
#include "Framework/ShaderTestDescriptorManager.h"

#include "Utility/Tuple.h"

namespace stf
{
    ShaderTestDescriptorManager::ShaderTestDescriptorManager(ObjectToken InToken, CreationParams InParams)
        : Object(InToken)
        , m_Device(InParams.Device)
        , m_GPUHeap(InParams.Device->CreateDescriptorHeap(
            D3D12_DESCRIPTOR_HEAP_DESC
            {
                .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                .NumDescriptors = InParams.InitialSize,
                .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
                .NodeMask = 0
            }))
        , m_CPUHeap(InParams.Device->CreateDescriptorHeap(
            D3D12_DESCRIPTOR_HEAP_DESC
            {
                .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                .NumDescriptors = InParams.InitialSize,
                .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                .NodeMask = 0
            }))
        , m_Allocator({.NumDescriptors = InParams.InitialSize})
        , m_CPUDescriptors(m_CPUHeap->GetHeapRange())
        , m_GPUDescriptors(m_GPUHeap->GetHeapRange())
    {
    }

    ShaderTestDescriptorManager::Expected<ShaderTestUAV> ShaderTestDescriptorManager::CreateUAV(SharedPtr<GPUResource> InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc)
    {
        using ErrorType = BindlessFreeListAllocator::EErrorType;
        return m_Allocator.Allocate()
            .transform(
                [this, resource = std::move(InResource), &InDesc](const BindlessFreeListAllocator::BindlessIndex InHandle) mutable
                {

                    m_Device->CreateUnorderedAccessView(*resource, InDesc, ThrowIfUnexpected(m_CPUDescriptors[InHandle.GetIndex()]));
                    return ShaderTestUAV
                    {
                        .Resource = std::move(resource),
                        .Handle = InHandle
                    };
                })
            .transform_error(
                [](const ErrorType InError)
                {
                    ThrowIfFalse(InError == ErrorType::EmptyError);

                    return EErrorType::AllocatorFull;
                });
    }

    ShaderTestDescriptorManager::Expected<void> ShaderTestDescriptorManager::ReleaseUAV(const ShaderTestUAV& InUAV)
    {
        using ErrorType = BindlessFreeListAllocator::EErrorType;
        return
            m_Allocator.Release(InUAV.Handle)
            .transform_error(
                [](const ErrorType InError)
                {
                    ThrowIfFalse(InError == ErrorType::IndexAlreadyReleased);
                    return EErrorType::DescriptorAlreadyFree;
                }
            );
    }

    ShaderTestDescriptorManager::Expected<SharedPtr<DescriptorHeap>> ShaderTestDescriptorManager::Resize(const u32 InNewSize)
    {
        if (m_Allocator.GetCapacity() >= InNewSize)
        {
            return Unexpected(EErrorType::AttemptedShrink);
        }

        auto copyDescriptorsToNewHeap = 
            [this, InNewSize](const DescriptorRange InSrc, const D3D12_DESCRIPTOR_HEAP_FLAGS InFlags)
            {
                auto newHeap = m_Device->CreateDescriptorHeap(
                    D3D12_DESCRIPTOR_HEAP_DESC
                    {
                        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                        .NumDescriptors = InNewSize,
                        .Flags = InFlags,
                        .NodeMask = 0
                    });
                const auto destRange = newHeap->GetHeapRange();
                m_Device->CopyDescriptors(destRange, InSrc, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

                return Tuple{ destRange, std::move(newHeap) };
            };

        const auto oldGPUHeap = m_GPUHeap;

        stf::tie(m_CPUDescriptors, m_CPUHeap) = copyDescriptorsToNewHeap(m_CPUDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        stf::tie(m_GPUDescriptors, m_GPUHeap) = copyDescriptorsToNewHeap(m_CPUDescriptors, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        m_Allocator.Resize(InNewSize);

        return oldGPUHeap;
    }

    u32 ShaderTestDescriptorManager::GetSize() const
    {
        return m_Allocator.GetSize();
    }

    u32 ShaderTestDescriptorManager::GetCapacity() const
    {
        return m_Allocator.GetCapacity();
    }

    void ShaderTestDescriptorManager::SetDescriptorHeap(CommandList& InCommandList)
    {
        m_Device->CopyDescriptors(m_GPUDescriptors, m_CPUDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        InCommandList.SetDescriptorHeaps(*m_GPUHeap);
    }
}