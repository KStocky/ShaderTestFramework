#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_ZeroAssertBuffer_WHEN_ZeroAssertsMade_THEN_HasExpectedResults()
{
}

[numthreads(1, 1, 1)]
void GIVEN_ZeroAssertBuffer_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults()
{
    ASSERT(IsTrue, true);
    ASSERT(IsFalse, false);
}

[numthreads(1, 1, 1)]
void GIVEN_ZeroAssertBuffer_WHEN_NonZeroFailedAssertsMade_THEN_HasExpectedResults()
{
    ASSERT(IsTrue, false);
    ASSERT(IsFalse, true);
}

[numthreads(1, 1, 1)]
void GIVEN_ZeroAssertBuffer_WHEN_NonZeroSuccessfulAndFailedAssertsMade_THEN_HasExpectedResults()
{
    ASSERT(IsTrue, true);
    ASSERT(IsFalse, false);
    ASSERT(IsTrue, false);
    ASSERT(IsFalse, true);
}