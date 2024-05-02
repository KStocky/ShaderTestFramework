#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	int a = __COUNTER__;
	int b = __COUNTER__;
	if (a == (b - 1))
	{
		ShaderTestPrivate::Success();
	}
	else
	{
		STF::Fail();
	}
}