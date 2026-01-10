#pragma once

#include "Platform.h"

#include "Container/RingBuffer.h"
#include "Utility/Error.h"

#include <compare>
#include <vector>

namespace stf
{
    namespace Errors
    {
        ErrorFragment BindlessFreeListAllocatorIsEmpty();
        ErrorFragment UnknownBindlessFreeListAllocatorError();

        ErrorFragment InvalidBindlessIndex(const u32 InIndex);
        ErrorFragment BindlessIndexAlreadyReleased(const u32 InIndex);
        ErrorFragment ShrinkAttemptedOnBindlessAllocator(const u32 InCurrentSize, const u32 InRequestedSize);
    }

    class BindlessFreeListAllocator
    {
        struct Private { explicit Private() = default; };
    public:

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

        [[nodiscard]] ExpectedError<BindlessIndex> Allocate();
        ExpectedError<void> Release(const BindlessIndex InIndex);
        ExpectedError<void> Resize(const u32 InNewSize);

        u32 GetSize() const;
        u32 GetCapacity() const;

        ExpectedError<bool> IsAllocated(const BindlessIndex InIndex) const;

    private:

        using EBufferError = RingBuffer<u32>::EErrorType;

        RingBuffer<u32> m_FreeList;
        std::vector<bool> m_FreeSet;
        u32 m_NumDescriptors = 0;
    };
}