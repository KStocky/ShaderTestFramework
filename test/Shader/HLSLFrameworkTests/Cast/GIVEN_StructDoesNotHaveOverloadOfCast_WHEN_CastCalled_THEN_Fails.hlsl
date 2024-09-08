#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructDoesNotHaveOverloadOfCast_WHEN_CastCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 42;
    int result = ttl::cast<int>(test);
    ASSER(AreEqual, result, 42);
}