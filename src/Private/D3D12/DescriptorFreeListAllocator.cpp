#include "D3D12/DescriptorFreeListAllocator.h"

#include <algorithm>
#include <iterator>

namespace stf
{
    DescriptorFreeListAllocator::DescriptorFreeListAllocator(const DescriptorRange& InRange)
        : m_Range(InRange)
        , m_FreeList(InRange.GetSize())
        , m_FreeSet(InRange.GetSize(), 1)
    {
        std::ranges::generate_n(std::back_inserter(m_FreeList), InRange.GetSize(), [index = 0]() mutable { return index++; });
    }

    DescriptorFreeListAllocator::Expected<DescriptorHandle> DescriptorFreeListAllocator::Allocate()
    {
        if (m_FreeList.size() == 0)
        {
            return Unexpected(EErrorType::EmptyError);
        }

        const auto index = m_FreeList.pop_front();
        m_FreeSet[*index] = 0;
        return *m_Range[*index];
    }

    DescriptorFreeListAllocator::Expected<void> DescriptorFreeListAllocator::Release(const DescriptorHandle& InDescriptor)
    {
        const auto index = m_Range.GetIndex(InDescriptor);

        if (!index.has_value() || m_FreeSet[*index] != 0)
        {
            return Unexpected(EErrorType::InvalidDescriptor);
        }

        m_FreeList.push_back(*index);
        m_FreeSet[*index] = 1;

        return Expected<void>{};
    }

    u32 DescriptorFreeListAllocator::GetSize() const
    {
        return GetCapacity() - static_cast<u32>(m_FreeList.size());
    }

    u32 DescriptorFreeListAllocator::GetCapacity() const
    {
        return m_Range.GetSize();
    }
}
