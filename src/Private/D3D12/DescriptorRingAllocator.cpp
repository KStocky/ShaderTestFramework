#include "D3D12/DescriptorRingAllocator.h"

namespace stf
{
    DescriptorRingAllocator::DescriptorRingAllocator(const DescriptorRange& InRange)
        : m_Range(InRange)
        , m_Size(0)
    {
    }

    DescriptorRingAllocator::Expected<DescriptorRange> DescriptorRingAllocator::Allocate(const u32 InNum)
    {
        const auto [index, numAllocated] =
            [&]() -> std::pair<u32, u32>
            {
                const u32 capacity = GetCapacity();
                const bool needToWrapToBeginning = m_AllocateIndex + InNum > capacity;

                if (needToWrapToBeginning)
                {
                    return { 0, InNum + (capacity - m_AllocateIndex) };
                }
                else
                {
                    return { m_AllocateIndex, InNum };
                }
            }();

        if (!CanAllocateInternal(numAllocated))
        {
            return Unexpected(EErrorType::AllocationError);
        }

        m_Size += numAllocated;
        m_AllocateIndex = index + InNum;

        return *m_Range.Subrange(index, InNum);
    }

    DescriptorRingAllocator::Expected<void> DescriptorRingAllocator::Release(const u32 InNum)
    {
        if (InNum > m_Size)
        {
            return Unexpected(EErrorType::ReleaseError);
        }
        m_ReleaseIndex = (m_ReleaseIndex + InNum) % GetCapacity();
        m_Size -= InNum;

        return {};
    }

    u32 DescriptorRingAllocator::GetSize() const
    {
        return m_Size;
    }

    u32 DescriptorRingAllocator::GetCapacity() const
    {
        return m_Range.GetSize();
    }

    bool DescriptorRingAllocator::CanAllocate(const u32 InNum) const
    {
        return CanAllocateInternal(CalculateAllocationIndexAndSize(InNum).second);
    }

    bool DescriptorRingAllocator::CanAllocateInternal(const u32 InActualNum) const
    {
        return InActualNum + m_Size <= GetCapacity();
    }

    std::pair<u32, u32> DescriptorRingAllocator::CalculateAllocationIndexAndSize(const u32 InRequestedNum) const
    {
        const u32 capacity = GetCapacity();
        const bool needToWrapToBeginning = m_AllocateIndex + InRequestedNum > capacity;

        if (needToWrapToBeginning)
        {
            return { 0, InRequestedNum + (capacity - m_AllocateIndex) };
        }
        else
        {
            return { m_AllocateIndex, InRequestedNum };
        }
    }
}
