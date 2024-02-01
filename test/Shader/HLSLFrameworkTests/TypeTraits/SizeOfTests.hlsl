#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

enum A
{
    One
};

enum class B
{
    One
};

struct C
{
    int D;
};

STATIC_ASSERT((4u == ttl::size_of<A>::value));
STATIC_ASSERT((4u == ttl::size_of<B>::value));
STATIC_ASSERT((4u == ttl::size_of<C>::value));
STATIC_ASSERT((4u == ttl::size_of<uint>::value));
STATIC_ASSERT((8u == ttl::size_of<uint64_t>::value));
STATIC_ASSERT((2u == ttl::size_of<uint16_t>::value));
STATIC_ASSERT((24u == ttl::size_of<uint64_t3>::value));
STATIC_ASSERT((8u == ttl::size_of<uint16_t4>::value));