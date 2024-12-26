
#include "D3D12/BindlessFreeListAllocator.h"

#include <algorithm>

namespace stf
{
    BindlessFreeListAllocator::BindlessFreeListAllocator(CreationParams InParams)
        : m_FreeList(InParams.NumDescriptors)
        , m_FreeSet(InParams.NumDescriptors, true)
        , m_NumDescriptors(InParams.NumDescriptors)
    {
        std::ranges::generate_n(std::back_inserter(m_FreeList), m_NumDescriptors, [index = 0]() mutable { return index++; });
    }

    BindlessFreeListAllocator::Expected<BindlessFreeListAllocator::BindlessIndex> BindlessFreeListAllocator::Allocate()
    {
        return m_FreeList.pop_front()
            .transform(
                [this](const u32 InIndex)
                {
                    m_FreeSet[InIndex] = false;
                    return BindlessIndex{ Private{}, InIndex };
                })
            .transform_error(
                [](const EBufferError InError)
                {
                    switch (InError)
                    {
                        case EBufferError::EmptyBuffer:
                        {
                            return EErrorType::EmptyError;
                        }

                        default:
                        {
                            return EErrorType::UnknownError;
                        }
                    }
                }
            );
    }

    BindlessFreeListAllocator::Expected<void> BindlessFreeListAllocator::Release(const BindlessIndex InIndex)
    {
        const u32 index = InIndex;
        if (index >= m_NumDescriptors)
        {
            return Unexpected(EErrorType::InvalidIndex);
        }

        if (m_FreeSet[index])
        {
            return Unexpected(EErrorType::IndexAlreadyReleased);
        }

        m_FreeList.push_back(index);
        m_FreeSet[index] = true;

        return {};
    }

    BindlessFreeListAllocator::Expected<void> BindlessFreeListAllocator::Resize(const u32 InNewSize)
    {
        if (InNewSize < m_NumDescriptors)
        {
            return Unexpected(EErrorType::ShrinkAttempted);
        }

        if (InNewSize == m_NumDescriptors)
        {
            return {};
        }

        const u32 numAdded = InNewSize - m_NumDescriptors;

        m_FreeList.resize(InNewSize);
        m_FreeSet.reserve(InNewSize);
        std::ranges::generate_n(std::back_inserter(m_FreeList), numAdded, [index = m_NumDescriptors]() mutable { return index++; });
        std::ranges::generate_n(std::back_inserter(m_FreeSet), numAdded, []() { return true; });
        m_NumDescriptors = InNewSize;

        return {};
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
}