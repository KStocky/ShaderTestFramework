#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

template<typename T>
struct A
{
    STATIC_ASSERT((TEST_CONDITION));
};

[numthreads(1,1,1)]
void Main()
{
}