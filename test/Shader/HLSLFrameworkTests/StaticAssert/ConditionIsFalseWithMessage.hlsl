#include "/Test/TTL/static_assert.hlsli"

[numthreads(1,1,1)]
void Main()
{
    STATIC_ASSERT(false, "This should succeed");
}