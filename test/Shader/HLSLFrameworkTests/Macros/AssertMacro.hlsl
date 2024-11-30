#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_TwoEqualInts_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, 4, 4);
}

[numthreads(1,1,1)]
void GIVEN_TwoNotEqualInts_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, 4, 5);
}

[numthreads(1,1,1)]
void GIVEN_TwoEqualInts_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(NotEqual, 4, 4);
}

[numthreads(1,1,1)]
void GIVEN_TwoNotEqualInts_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(NotEqual, 4, 5);
}

[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, true);
}

[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, false);
}

[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, true);
}

[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, false);
}

[numthreads(1,1,1)]
void GIVEN_TestWithFailAssert_WHEN_Ran_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(Fail);
}