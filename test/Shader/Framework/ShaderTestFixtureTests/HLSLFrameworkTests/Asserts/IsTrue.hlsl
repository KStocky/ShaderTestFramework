#include "/Test/Public/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Value;
};

namespace STF
{
    template<>
    bool Cast<bool, TestStruct>(TestStruct In)
    {
        return In.Value == 0;
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsTrue(true);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueNonLiteral_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = true;
    STF::IsTrue(value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TrueExpression_WHEN_IsTrueCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsTrue(2 == 2);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsTrue(false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseNonLiteral_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = false;
    STF::IsTrue(value);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FalseExpression_WHEN_IsTrueCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::IsTrue(2 == 3);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 0;
    STF::IsTrue(test);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 1;
    STF::IsTrue(test);
}