#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_TwoCallsToCounter_WHEN_Compared_THEN_AreDifferent(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	int a = __COUNTER__;
	int b = __COUNTER__;
	if (a == (b - 1))
	{
		stf::detail::Success();
	}
	else
	{
        ASSERT(Fail);
    }
}