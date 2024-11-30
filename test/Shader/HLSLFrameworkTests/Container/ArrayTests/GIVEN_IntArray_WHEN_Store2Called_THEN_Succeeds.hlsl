#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_Store2Called_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ttl::container_wrapper<int[2]> span;
    span.store(0, 42, 53);

    ASSERT(AreEqual, 42, span.Data[0]);
    ASSERT(AreEqual, 53, span.Data[1]);
}