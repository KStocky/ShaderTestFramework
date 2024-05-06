
#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

struct A
{
};

struct B
{
};

STATIC_ASSERT((ttl::is_same<A, __decltype(ttl::declval<A>())>::value));
STATIC_ASSERT((!ttl::is_same<B, __decltype(ttl::declval<A>())>::value));

STATIC_ASSERT((!ttl::is_same<A, __decltype(ttl::declval<B>())>::value));
STATIC_ASSERT((ttl::is_same<B, __decltype(ttl::declval<B>())>::value));