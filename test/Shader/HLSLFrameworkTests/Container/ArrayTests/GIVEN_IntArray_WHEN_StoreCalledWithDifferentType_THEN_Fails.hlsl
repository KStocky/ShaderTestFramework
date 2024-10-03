#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_StoreCalledWithDifferentType_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ttl::container_wrapper<int[2]> span;
    span.store(0, 42u);
}