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
void GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsFalse(true);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueNonLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = true;
    STF::IsFalse(value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueExpression_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsFalse(2 == 2);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsFalse(false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = false;
    STF::IsFalse(value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsFalse(2 == 3);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 0;
    STF::IsFalse(test);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 1;
    STF::IsFalse(test);
}