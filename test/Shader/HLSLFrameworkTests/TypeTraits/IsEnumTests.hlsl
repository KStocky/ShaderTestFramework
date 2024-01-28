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

[numthreads(1,1,1)]
void AllSucceed()
{
    STATIC_ASSERT((ttl::is_enum<A>::value));
    STATIC_ASSERT((ttl::is_enum<B>::value));
    STATIC_ASSERT((!ttl::is_enum<C>::value));
    STATIC_ASSERT((!ttl::is_enum<uint>::value));
}