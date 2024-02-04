#pragma once

namespace STF
{
    template<uint16_t InReaderId, uint16_t InTypeId = 0>
    struct ByteReaderTraitsBase
    {
        static const uint16_t ReaderId = InReaderId;
        static const uint16_t TypeId = InTypeId;
    };

    template<typename T>
    struct ByteReaderTraits : ByteReaderTraitsBase<0, 0>
    {
    };
}