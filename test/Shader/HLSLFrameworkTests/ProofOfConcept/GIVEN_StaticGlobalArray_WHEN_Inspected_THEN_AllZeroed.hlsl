#include "/Test/STF/ShaderTestFramework.hlsli"

struct StaticArrayTest
{
	int Val;
	bool Other;
};

static StaticArrayTest globalArray[100];

[numthreads(1,1,1)]
void GIVEN_StaticGlobalArray_WHEN_Inspected_THEN_AllZeroed(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	for (int i = 0; i < 100; ++i)
	{
		ASSERT(AreEqual, globalArray[i].Val, 0);
		ASSERT(IsFalse, globalArray[i].Other);
	}
}