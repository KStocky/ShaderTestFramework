#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, 4, 4);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, 4, 5);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(NotEqual, 4, 4);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(NotEqual, 4, 5);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, true);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, true);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(Fail);
}