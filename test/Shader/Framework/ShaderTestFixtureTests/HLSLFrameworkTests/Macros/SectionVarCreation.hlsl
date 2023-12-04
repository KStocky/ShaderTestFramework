#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::AreEqual(1, ShaderTestPrivate::NextSectionID);
    STF_CREATE_SECTION_VAR;

    STF::AreEqual(2, ShaderTestPrivate::NextSectionID);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::AreEqual(1, ShaderTestPrivate::NextSectionID);
    STF_CREATE_SECTION_VAR;
    STF_CREATE_SECTION_VAR;
    STF::AreEqual(3, ShaderTestPrivate::NextSectionID);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::AreEqual(1, ShaderTestPrivate::NextSectionID);
    
    for (int i = 0; i < 3; ++i)
    {
        STF_CREATE_SECTION_VAR;
        STF_CREATE_SECTION_VAR;
    }

    STF::AreEqual(3, ShaderTestPrivate::NextSectionID);
}