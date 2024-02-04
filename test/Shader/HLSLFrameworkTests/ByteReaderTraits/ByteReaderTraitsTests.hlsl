#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/TTL/static_assert.hlsli"

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

namespace STF
{
    template<> struct ByteReaderTraits<TypeWithReaderTraitsNoTypeId> : ByteReaderTraitsBase<TypeWithReaderTraitsNoTypeId::ReaderId>{};
    template<> struct ByteReaderTraits<TypeWithReaderTraitsAndTypeId> : ByteReaderTraitsBase<TypeWithReaderTraitsAndTypeId::ReaderId, TypeWithReaderTraitsAndTypeId::TypeId>{};
}

STATIC_ASSERT(STF::ByteReaderTraits<TypeWithoutReaderTraits>::ReaderId == 0);
STATIC_ASSERT(STF::ByteReaderTraits<TypeWithoutReaderTraits>::TypeId == 0);

STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsNoTypeId>::ReaderId == TypeWithReaderTraitsNoTypeId::ReaderId);
STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsNoTypeId>::TypeId == 0);

STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsAndTypeId>::ReaderId == TypeWithReaderTraitsAndTypeId::ReaderId);
STATIC_ASSERT(STF::ByteReaderTraits<TypeWithReaderTraitsAndTypeId>::TypeId == TypeWithReaderTraitsAndTypeId::TypeId);