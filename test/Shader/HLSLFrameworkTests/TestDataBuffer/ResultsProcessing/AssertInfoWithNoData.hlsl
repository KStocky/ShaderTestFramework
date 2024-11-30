#include "/Test/stf/ShaderTestFramework.hlsli"

struct TestType
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

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_ZeroAssertsMade_THEN_HasExpectedResults()
{
}

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_NonZeroSuccessfulAssertsMade_THEN_HasExpectedResults()
{
    ASSERT(IsTrue, true);
    ASSERT(IsFalse, false);
}

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_FailedAssertNoTypeId_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    stf::IsTrue(t, 42);
}

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_TwoFailedAssert_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    stf::IsTrue(t, 42);
    stf::IsTrue(t, 32);
}

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_FailedAssertWithLineId_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    ASSERT(IsTrue, t);
}

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacityWithFlatThreadId_WHEN_FailedAssert_THEN_HasExpectedResults()
{
    stf::RegisterThreadID(12);
    TestType t;
    t.Value = false;
    stf::IsTrue(t, 66);
}

[numthreads(24, 1, 1)]
void GIVEN_AssertInfoCapacityWithFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);
    
    if (DispatchThreadId.x == 12)
    {
        TestType t;
        t.Value = false;
        stf::IsTrue(t, 66);
    }
}

[numthreads(3, 3, 3)]
void GIVEN_AssertInfoCapacityWithNonFlat3DThreadId_WHEN_FailedAssert_THEN_HasExpectedResults(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);
    
    if (DispatchThreadId.x == 1 && DispatchThreadId.y == 1 && DispatchThreadId.z == 0)
    {
        TestType t;
        t.Value = false;
        stf::IsTrue(t, 66);
    }
}

[numthreads(1, 1, 1)]
void GIVEN_AssertInfoCapacity_WHEN_MoreFailedAssertsThanCapacity_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = false;
    stf::IsTrue(t, 42);
    stf::IsTrue(t, 42);
}
