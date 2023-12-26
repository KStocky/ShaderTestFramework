#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestType
{
    bool Value;
};

struct TestTypeWithTypeId
{
    bool Value;
};

namespace ttl
{
    template<>
    struct caster<bool, TestType>
    {
        static bool cast(TestType In)
        {
            return In.Value;
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_ZeroAssertsMade_THEN_HasExpectedResults()
{
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults()
{
    ASSERT(IsTrue, true);
    ASSERT(IsFalse, false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_FailedAssertNoTypeId_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_TwoFailedAssert_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    STF::IsTrue(t, 42);
    STF::IsTrue(t, 32);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_FailedAssertWithLineId_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    ASSERT(IsTrue, t);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacityWithFlatThreadId_WHEN_FailedAssert_THEN_HasExpectedResults()
{
    STF::RegisterThreadID(12);
    TestType t;
    t.Value = false;
    STF::IsTrue(t, 66);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(24, 1, 1)]
void GIVEN_AssertInfoCapacityWithFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::RegisterThreadID(DispatchThreadId);
    
    if (DispatchThreadId.x == 12)
    {
        TestType t;
        t.Value = false;
        STF::IsTrue(t, 66);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(3, 3, 3)]
void GIVEN_AssertInfoCapacityWithNonFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::RegisterThreadID(DispatchThreadId);
    
    if (DispatchThreadId.x == 1 && DispatchThreadId.y == 1 && DispatchThreadId.z == 0)
    {
        TestType t;
        t.Value = false;
        STF::IsTrue(t, 66);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_MoreFailedAssertsThanCapacity_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    STF::IsTrue(t, 42);
    STF::IsTrue(t, 42);
}
