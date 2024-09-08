#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
};

namespace ttl
{
    template<>
    struct caster<bool, TestStruct>
    {
        static bool cast(TestStruct In)
        {
            return In.Value == 0;
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, true);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueNonLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = true;
    ASSERT(IsTrue, value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueExpression_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, 2 == 2);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = false;
    ASSERT(IsTrue, value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsTrue, 2 == 3);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 0;
    ASSERT(IsTrue, test);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 1;
    ASSERT(IsTrue, test);
}