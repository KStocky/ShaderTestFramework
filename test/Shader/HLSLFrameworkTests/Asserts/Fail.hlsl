#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::Fail();
}

[numthreads(1,1,1)]
void GIVEN_Empty_WHEN_Ran_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
}