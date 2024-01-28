#include "/Test/TTL/static_assert.hlsli"

struct A
{
    STATIC_ASSERT(TEST_CONDITION);
};

[numthreads(1,1,1)]
void Main()
{
}