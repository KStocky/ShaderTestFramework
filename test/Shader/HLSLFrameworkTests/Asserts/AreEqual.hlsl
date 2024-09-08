#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
    bool operator==(TestStruct InOther)
    {
        return Value == InOther.Value;
    }
};

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
void GIVEN_TwoEqualFloat4_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, float4(1.0, 2.5, 3.5, 5.75), float4(1.0, 2.5, 3.5, 5.75));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualFloat4_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(AreEqual, float4(1.0, 2.5, 3.5, 5.75), float4(2.0, 2.5, 3.5, 5.75));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoEqualNamedInts_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const int expected = 4;
    ASSERT(AreEqual, 4, expected);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualNamedInts_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const int expected = 5;
    ASSERT(AreEqual, 4, expected);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const float4 expected = float4(1.0, 2.5, 3.5, 5.75);
    ASSERT(AreEqual, float4(1.0, 2.5, 3.5, 5.75), expected);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNotEqualNamedFloat4_WHEN_AreEqualCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static const float4 expected = float4(2.0, 2.5, 3.5, 5.75);
    ASSERT(AreEqual, float4(1.0, 2.5, 3.5, 5.75), expected);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreEqual_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct left;
    TestStruct right;

    left.Value = 42;
    right.Value = 42;
    ASSERT(AreEqual, left, right);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoObjectsWithEqualOperatorOverloads_WHEN_ObjectsAreNotEqual_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct left;
    TestStruct right;

    left.Value = 42;
    right.Value = 43;
    ASSERT(AreEqual, left, right);
}