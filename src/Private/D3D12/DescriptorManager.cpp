
#include "D3D12/DescriptorManager.h"

namespace stf
{
    DescriptorManager::Descriptor::Descriptor(DescriptorManager::Token, const SharedPtr<DescriptorManager>& InOwner, BindlessFreeListAllocator::BindlessIndex InIndex)
        : m_Owner(InOwner)
        , m_Index(InIndex)
    {
    }

    DescriptorManager::Expected<DescriptorHandle> DescriptorManager::Descriptor::Resolve() const
    {
        return m_Owner->ResolveDescriptor(m_Index);
    }

    DescriptorManager* DescriptorManager::Descriptor::GetOwner(Token) const
    {
        return m_Owner.get();
    }

    BindlessFreeListAllocator::BindlessIndex DescriptorManager::Descriptor::GetIndex(Token) const
    {
        return m_Index;
    }

    DescriptorManager::DescriptorManager(ObjectToken InToken, const CreationParams& InParams)
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
        , m_Allocator({ .NumDescriptors = InParams.InitialSize })
    {
    }

    DescriptorManager::Expected<DescriptorManager::Descriptor> DescriptorManager::Acquire()
    {
        return m_Allocator.Allocate()
            .transform(
                [this](const BindlessFreeListAllocator::BindlessIndex InHandle) mutable
                {
                    return Descriptor{ Token{}, SharedFromThis(), InHandle };
                })
            .transform_error(
                [](const BindlessFreeListAllocator::EErrorType InError)
                {
                    ThrowIfFalse(InError == BindlessFreeListAllocator::EErrorType::EmptyError);

                    return EErrorType::AllocatorFull;
                });
    }

    DescriptorManager::Expected<void> DescriptorManager::Release(const Descriptor& InDescriptor)
    {
        return
            m_Allocator.Release(InDescriptor.GetIndex(Token{}))
            .transform_error(
                [](const BindlessFreeListAllocator::EErrorType InError)
                {
                    ThrowIfFalse(InError == BindlessFreeListAllocator::EErrorType::IndexAlreadyReleased);
                    return EErrorType::DescriptorAlreadyFree;
                }
            );
    }

    DescriptorManager::Expected<SharedPtr<DescriptorHeap>> DescriptorManager::Resize(const u32 InNewSize)
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

                return newHeap;
            };

        auto oldGPUHeap = m_GPUHeap;

        m_CPUHeap = copyDescriptorsToNewHeap(m_CPUHeap->GetHeapRange(), D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
        m_GPUHeap = copyDescriptorsToNewHeap(m_CPUHeap->GetHeapRange(), D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
        return m_Allocator.Resize(InNewSize)
            .transform(
                [oldHeap = std::move(oldGPUHeap)]()
                {
                    return std::move(oldHeap);
                }
            ).transform_error(
                [](const BindlessFreeListAllocator::EErrorType InError)
                {
                    ThrowIfFalse(InError == BindlessFreeListAllocator::EErrorType::ShrinkAttempted);
                    return EErrorType::AttemptedShrink;
                }
            );
    }

    u32 DescriptorManager::GetSize() const
    {
        return m_Allocator.GetSize();
    }

    u32 DescriptorManager::GetCapacity() const
    {
        return m_Allocator.GetCapacity();
    }

    void DescriptorManager::SetDescriptorHeap(CommandList& InCommandList)
    {
        m_Device->CopyDescriptors(m_GPUHeap->GetHeapRange(), m_CPUHeap->GetHeapRange(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        InCommandList.SetDescriptorHeaps(*m_GPUHeap);
    }

    DescriptorManager::Expected<DescriptorHandle> DescriptorManager::ResolveDescriptor(const BindlessFreeListAllocator::BindlessIndex InIndex) const
    {
        return m_Allocator
            .IsAllocated(InIndex)
            .transform_error(
                [](const BindlessFreeListAllocator::EErrorType InError)
                {
                    ThrowIfFalse(InError == BindlessFreeListAllocator::EErrorType::InvalidIndex);

                    return EErrorType::DescriptorInvalid;
                }
            )
            .and_then(
                [this, InIndex](const bool InIsAllocated) -> Expected<DescriptorHandle>
                {
                    if (InIsAllocated)
                    {
                        const auto descriptorRange = m_CPUHeap->GetHeapRange();

                        return descriptorRange[InIndex.GetIndex()]
                            .transform_error(
                                [](const DescriptorRange::EErrorType InError)
                                {
                                    ThrowIfFalse(InError == DescriptorRange::EErrorType::InvalidIndex);

                                    return EErrorType::DescriptorInvalid;
                                }
                            );
                    }
                    else
                    {
                        return Unexpected{ EErrorType::DescriptorNotAllocated };
                    }
                }
            );
            
    }
}
