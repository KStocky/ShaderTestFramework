#include "/Test/stf/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_SingleThreadDispatched_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = stf::detail::FlattenIndex(DispatchThreadId, stf::detail::DispatchDimensions);

    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int3, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(1,1,1)]
void GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = stf::detail::FlattenIndex(DispatchThreadId, stf::detail::DispatchDimensions);

    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int3, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(10,10,10)]
void GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = stf::detail::FlattenIndex(DispatchThreadId, stf::detail::DispatchDimensions);

    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int3, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(2,2,2)]
void GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = stf::detail::FlattenIndex(DispatchThreadId, stf::detail::DispatchDimensions);

    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int3, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(1,1,1)]
void GIVEN_ThreadIDGivenAsUint_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = 42u;

    stf::RegisterThreadID(expectedData);

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(1,1,1)]
void GIVEN_ThreadIdNotRegistered_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint expectedData = 0;
    stf::detail::Scratch.Init();

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::None, stf::detail::Scratch.ThreadID.Type);
}
