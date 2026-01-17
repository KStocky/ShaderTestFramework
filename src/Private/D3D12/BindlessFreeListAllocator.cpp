
#include "D3D12/BindlessFreeListAllocator.h"

#include <algorithm>

namespace stf
{
    namespace Errors::BindlessFreeListAllocator
    {
        ErrorFragment Empty()
        {
            return ErrorFragment::Make<"Bindless allocator is empty">();
        }

        ErrorFragment InvalidIndex(const u32 InIndex)
        {
            return ErrorFragment::Make<"Bindless index {} is invalid">(InIndex);
        }

        ErrorFragment IndexAlreadyReleased(const u32 InIndex)
        {
            return ErrorFragment::Make<"Bindless index {} has already been released">(InIndex);
        }

        ErrorFragment ShrinkAttempted(const u32 InCurrentSize, const u32 InRequestedSize)
        {
            return ErrorFragment::Make<"Attempted shrink which is unsupported. Current size: {}, Requested size: {}">(InCurrentSize, InRequestedSize);
        }
    }

    BindlessFreeListAllocator::BindlessFreeListAllocator(CreationParams InParams)
        : m_FreeList(InParams.NumDescriptors)
        , m_FreeSet(InParams.NumDescriptors, true)
        , m_NumDescriptors(InParams.NumDescriptors)
    {
        std::ranges::generate_n(std::back_inserter(m_FreeList), m_NumDescriptors, [index = 0]() mutable { return index++; });
    }

    ExpectedError<BindlessFreeListAllocator::BindlessIndex> BindlessFreeListAllocator::Allocate()
    {
        return m_FreeList.pop_front()
            .transform(
                [this](const u32 InIndex)
                {
                    m_FreeSet[InIndex] = false;
                    return BindlessIndex{ Private{}, InIndex };
                })
            .transform_error(
                [](const Error& InError) -> Error
                {
                    return InError + Errors::BindlessFreeListAllocator::Empty();
                }
            );
    }

    ExpectedError<void> BindlessFreeListAllocator::Release(const BindlessIndex InIndex)
    {
        const u32 index = InIndex;
        if (index >= m_NumDescriptors)
        {
            return Unexpected{ Error{Errors::BindlessFreeListAllocator::InvalidIndex(index) } };
        }

        if (m_FreeSet[index])
        {
            return Unexpected{ Error{ Errors::BindlessFreeListAllocator::IndexAlreadyReleased(index) } };
        }

        m_FreeList.push_back(index);
        m_FreeSet[index] = true;

        return {};
    }

    ExpectedError<void> BindlessFreeListAllocator::Resize(const u32 InNewSize)
    {
        if (InNewSize < m_NumDescriptors)
        {
            return Unexpected{ Error{ Errors::BindlessFreeListAllocator::ShrinkAttempted(m_NumDescriptors, InNewSize) } };
        }

        if (InNewSize == m_NumDescriptors)
        {
            return {};
        }

        const u32 numAdded = InNewSize - m_NumDescriptors;

        return m_FreeList.resize(InNewSize)
            .transform(
                [this, numAdded, InNewSize]()
                {
                    m_FreeSet.reserve(InNewSize);
                    std::ranges::generate_n(std::back_inserter(m_FreeList), numAdded, [index = m_NumDescriptors]() mutable { return index++; });
                    std::ranges::generate_n(std::back_inserter(m_FreeSet), numAdded, []() { return true; });
                    m_NumDescriptors = InNewSize;
                }
            );
    }

    u32 BindlessFreeListAllocator::GetSize() const
    {
        return GetCapacity() - static_cast<u32>(m_FreeList.size());
    }

    u32 BindlessFreeListAllocator::GetCapacity() const
    {
        return m_NumDescriptors;
    }

    BindlessFreeListAllocator::BindlessIndex::BindlessIndex(Private, u32 InIndex)
        : m_Index(InIndex)
    {
    }

    BindlessFreeListAllocator::BindlessIndex::operator u32() const
    {
        return GetIndex();
    }

    u32 BindlessFreeListAllocator::BindlessIndex::GetIndex() const
    {
        return m_Index;
    }

    ExpectedError<bool> BindlessFreeListAllocator::IsAllocated(const BindlessIndex InIndex) const
    {
        const u32 index = InIndex;
        if (index >= m_NumDescriptors)
        {
            return Unexpected{ Error{ Errors::BindlessFreeListAllocator::InvalidIndex(InIndex) } };
        }

        return !m_FreeSet[InIndex];
    }
}