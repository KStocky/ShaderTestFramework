#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::Fail();
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_Empty_WHEN_Ran_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
}