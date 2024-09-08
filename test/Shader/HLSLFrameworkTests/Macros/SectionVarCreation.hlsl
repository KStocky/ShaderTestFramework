#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSectionVarCreated_WHEN_Queried_THEN_ValueIsZero(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::detail::Scratch.Init();
    ASSERT(AreEqual, 1, stf::detail::Scratch.NextSectionID);
    STF_CREATE_SECTION_VAR;

    ASSERT(AreEqual, 2, stf::detail::Scratch.NextSectionID);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSectionVarsCreated_WHEN_Queried_THEN_ValueAreAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::detail::Scratch.Init();
    ASSERT(AreEqual, 1, stf::detail::Scratch.NextSectionID);
    STF_CREATE_SECTION_VAR;
    STF_CREATE_SECTION_VAR;
    ASSERT(AreEqual, 3, stf::detail::Scratch.NextSectionID);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSectionVarsCreatedInALoop_WHEN_Queried_THEN_ValueAreAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::detail::Scratch.Init();
    ASSERT(AreEqual, 1, stf::detail::Scratch.NextSectionID);
    
    for (int i = 0; i < 3; ++i)
    {
        STF_CREATE_SECTION_VAR;
        STF_CREATE_SECTION_VAR;
    }

    ASSERT(AreEqual, 3, stf::detail::Scratch.NextSectionID);
}