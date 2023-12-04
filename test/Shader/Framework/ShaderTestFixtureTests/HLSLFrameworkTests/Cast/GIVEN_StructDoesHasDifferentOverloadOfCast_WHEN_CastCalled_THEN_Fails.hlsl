#include "/Test/Public/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
};

namespace STF
{
    template<>
    float Cast<float, TestStruct>(TestStruct In)
    {
        return (float)In.Value;
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 42;
    int result = STF::Cast<int>(test);
    STF::AreEqual(result, 42);
}