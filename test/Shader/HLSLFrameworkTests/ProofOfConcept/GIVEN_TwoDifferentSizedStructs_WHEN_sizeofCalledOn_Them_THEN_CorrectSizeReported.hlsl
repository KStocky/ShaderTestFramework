#include "/Test/STF/ShaderTestFramework.hlsli"

namespace SizeOfTests
{
	struct A
	{
		int A;
	};

	struct B
	{
		int A;
		int B;
	};
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoDifferentSizedStructs_WHEN_sizeofCalledOn_Them_THEN_CorrectSizeReported(uint3 DispatchThreadId : SV_DispatchThreadID)
{
	const uint expectedA = 4;
	const uint expectedB = 8;
	ASSERT(AreEqual, expectedA, (uint)sizeof(SizeOfTests::A));
	ASSERT(AreEqual, expectedB, (uint)sizeof(SizeOfTests::B));
}