#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleThreadDispatched_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = ShaderTestPrivate::FlattenIndex(DispatchThreadId, ShaderTestPrivate::DispatchDimensions);

    STF::RegisterThreadID(DispatchThreadId);

    STF::AreEqual(expectedData, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int3, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = ShaderTestPrivate::FlattenIndex(DispatchThreadId, ShaderTestPrivate::DispatchDimensions);

    STF::RegisterThreadID(DispatchThreadId);

    STF::AreEqual(expectedData, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int3, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(10,10,10)]
void GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = ShaderTestPrivate::FlattenIndex(DispatchThreadId, ShaderTestPrivate::DispatchDimensions);

    STF::RegisterThreadID(DispatchThreadId);

    STF::AreEqual(expectedData, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int3, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(2,2,2)]
void GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = ShaderTestPrivate::FlattenIndex(DispatchThreadId, ShaderTestPrivate::DispatchDimensions);

    STF::RegisterThreadID(DispatchThreadId);

    STF::AreEqual(expectedData, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int3, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ThreadIDGivenAsUint_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = 42u;

    STF::RegisterThreadID(expectedData);

    STF::AreEqual(expectedData, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ThreadIdNotRegistered_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = 0;
    ShaderTestPrivate::InitScratch();

    STF::AreEqual(expectedData, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::None, ShaderTestPrivate::Scratch.ThreadID.Type);
}
