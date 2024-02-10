#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/TTL/static_assert.hlsli"

namespace CustomByteReaderTraitsTests
{
    struct TypeWithoutReaderTraits
    {
    };

    struct TypeWithReaderTraitsNoTypeId
    {
        static const uint16_t ReaderId = 34;
    };

    struct TypeWithReaderTraitsAndTypeId
    {
        static const uint16_t ReaderId = 42;
        static const uint16_t TypeId = 124;
    }; 
}

namespace STF
{
    template<> struct ByteReaderTraits<CustomByteReaderTraitsTests::TypeWithReaderTraitsNoTypeId> 
        : ByteReaderTraitsBase<CustomByteReaderTraitsTests::TypeWithReaderTraitsNoTypeId::ReaderId>{};
    
    template<> struct ByteReaderTraits<CustomByteReaderTraitsTests::TypeWithReaderTraitsAndTypeId> 
        : ByteReaderTraitsBase<CustomByteReaderTraitsTests::TypeWithReaderTraitsAndTypeId::ReaderId, CustomByteReaderTraitsTests::TypeWithReaderTraitsAndTypeId::TypeId>{};
}

namespace CustomByteReaderTraitsTests
{
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithoutReaderTraits>::ReaderId == 0);
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithoutReaderTraits>::TypeId == 0);

    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsNoTypeId>::ReaderId == TypeWithReaderTraitsNoTypeId::ReaderId);
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsNoTypeId>::TypeId == 0);

    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsAndTypeId>::ReaderId == TypeWithReaderTraitsAndTypeId::ReaderId);
    STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsAndTypeId>::TypeId == TypeWithReaderTraitsAndTypeId::TypeId);
}