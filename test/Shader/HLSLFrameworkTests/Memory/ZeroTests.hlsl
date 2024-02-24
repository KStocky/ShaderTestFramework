#include "/Test/STF/ShaderTestFramework.hlsli"

struct Test
{
    uint a;
    float3 b;
    bool c;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectIsInitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed()
{
    Test t;
    t.a = 42;
    t.b = float3(1.0, 2.0, 3.0);
    t.c = true;

    ttl::zero(t);

    ASSERT(AreEqual, 0u, t.a);
    ASSERT(AreEqual, float3(0.0,0.0,0.0), t.b);
    ASSERT(AreEqual, false, t.c);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectIsUnitialized_WHEN_Zeroed_THEN_AllMembersAreZeroed()
{
    Test t;

    ttl::zero(t);

    ASSERT(AreEqual, 0u, t.a);
    ASSERT(AreEqual, float3(0.0,0.0,0.0), t.b);
    ASSERT(AreEqual, false, t.c);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NoObjectAlreadyExists_WHEN_Zeroed_THEN_AllMembersOfReturnedObjectAreZeroed()
{
    Test t = ttl::zero<Test>();

    ASSERT(AreEqual, 0u, t.a);
    ASSERT(AreEqual, float3(0.0,0.0,0.0), t.b);
    ASSERT(AreEqual, false, t.c);
}
