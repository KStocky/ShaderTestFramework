#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
};

namespace ttl
{
    template<>
    struct caster<float, TestStruct>
    {
        static float cast(TestStruct In)
        {
            return (float)In.Value;
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StructDoesHasDifferentOverloadOfCast_WHEN_CastCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 42;
    int result = ttl::cast<int>(test);
    ASSERT(AreEqual, result, 42);
}