#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
};

namespace ttl
{
    template<>
    struct caster<int, TestStruct>
    {
        static int cast(TestStruct In)
        {
            return In.Value;
        }
    };
}

[numthreads(1,1,1)]
void GIVEN_StructDoesHaveOverloadOfCast_WHEN_CastCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 42;
    int result = ttl::cast<int>(test);
    ASSERT(AreEqual, result, 42);
}