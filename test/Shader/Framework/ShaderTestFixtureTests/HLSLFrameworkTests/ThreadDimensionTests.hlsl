#include "/Test/Public/ShaderTestFramework.hlsli"

 [RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleThreadDispatched_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 expectedDim = uint3(1,1,1);

    STF::AreEqual(expectedDim, ShaderTestPrivate::DispatchDimensions);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 expectedDim = uint3(10,10,10);

    STF::AreEqual(expectedDim, ShaderTestPrivate::DispatchDimensions);
}
[RootSignature(SHADER_TEST_RS)]
[numthreads(10,10,10)]
void GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 expectedDim = uint3(10,10,10);

    STF::AreEqual(expectedDim, ShaderTestPrivate::DispatchDimensions);
}
[RootSignature(SHADER_TEST_RS)]
[numthreads(2,2,2)]
void GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchDimensionsQueried_THEN_IsAsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 expectedDim = uint3(4,4,4);

    STF::AreEqual(expectedDim, ShaderTestPrivate::DispatchDimensions);
}