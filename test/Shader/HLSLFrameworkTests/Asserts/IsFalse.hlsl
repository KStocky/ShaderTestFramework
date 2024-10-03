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

[numthreads(1,1,1)]
void GIVEN_TrueLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, true);
}

[numthreads(1,1,1)]
void GIVEN_TrueNonLiteral_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = true;
    ASSERT(IsFalse, value);
}

[numthreads(1,1,1)]
void GIVEN_TrueExpression_WHEN_IsFalseCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, 2 == 2);
}

[numthreads(1,1,1)]
void GIVEN_FalseLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, false);
}

[numthreads(1,1,1)]
void GIVEN_FalseNonLiteral_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const bool value = false;
    ASSERT(IsFalse, value);
}

[numthreads(1,1,1)]
void GIVEN_FalseExpression_WHEN_IsFalseCalled_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ASSERT(IsFalse, 2 == 3);
}

[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsTrue_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 0;
    ASSERT(IsFalse, test);
}

[numthreads(1,1,1)]
void GIVEN_ObjectWithBoolCast_WHEN_ObjectsIsFalse_THEN_Succeeds(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Value = 1;
    ASSERT(IsFalse, test);
}