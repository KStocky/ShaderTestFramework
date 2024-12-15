#pragma once

#include "Platform.h"

#include "Container/RingBuffer.h"
#include "Utility/Expected.h"

#include <compare>
#include <vector>

namespace stf
{
    class BindlessFreeListAllocator
    {
        struct Private { explicit Private() = default; };
    public:

        enum class EErrorType
        {
            UnknownError,
            EmptyError,
            InvalidIndex,
            IndexAlreadyReleased,
            ShrinkAttempted
        };

        template<typename T>
        using Expected = Expected<T, EErrorType>;

        class BindlessIndex
        {
        public:

            BindlessIndex(Private, u32 InIndex);

            operator u32() const;
            u32 GetIndex() const;

            friend auto operator<=>(const BindlessIndex&, const BindlessIndex&) = default;

        private:

            u32 m_Index = 0;
        };

        struct CreationParams
        {
            u32 NumDescriptors = 0;
        };

        BindlessFreeListAllocator() = default;
        BindlessFreeListAllocator(CreationParams InParams);

        [[nodiscard]] Expected<BindlessIndex> Allocate();
        Expected<void> Release(const BindlessIndex InIndex);
        Expected<void> Resize(const u32 InNewSize);

        u32 GetSize() const;
        u32 GetCapacity() const;

    private:

        using EBufferError = RingBuffer<u32>::EErrorType;

        RingBuffer<u32> m_FreeList;
        std::vector<bool> m_FreeSet;
        u32 m_NumDescriptors = 0;
    };
}