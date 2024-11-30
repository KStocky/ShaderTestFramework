#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
    bool operator!=(TestStruct InOther)
    {
        return Value != InOther.Value;
    }
};

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
void GIVEN_TwoEqualFloat4_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(NotEqual, float4(1.0, 2.5, 3.5, 5.75), float4(1.0, 2.5, 3.5, 5.75));
}
 
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualFloat4_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(NotEqual, float4(1.0, 2.5, 3.5, 5.75), float4(2.0, 2.5, 3.5, 5.75));
}

[numthreads(1,1,1)]
void GIVEN_TwoEqualNamedInts_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const int expected = 4;
    ASSERT(NotEqual, 4, expected);
}
 
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualNamedInts_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const int expected = 5;
    ASSERT(NotEqual, 4, expected);
}

[numthreads(1,1,1)]
void GIVEN_TwoEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const float4 expected = float4(1.0, 2.5, 3.5, 5.75);
    ASSERT(NotEqual, float4(1.0, 2.5, 3.5, 5.75), expected);
}
 
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualNamedFloat4_WHEN_NotEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const float4 expected = float4(2.0, 2.5, 3.5, 5.75);
    ASSERT(NotEqual, float4(1.0, 2.5, 3.5, 5.75), expected);
}

[numthreads(1,1,1)]
void GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct left;
    TestStruct right;

    left.Value = 42;
    right.Value = 42;
    ASSERT(NotEqual, left, right);
}

[numthreads(1,1,1)]
void GIVEN_TwoObjectsWithNotEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct left;
    TestStruct right;

    left.Value = 42;
    right.Value = 43;
    ASSERT(NotEqual, left, right);
}