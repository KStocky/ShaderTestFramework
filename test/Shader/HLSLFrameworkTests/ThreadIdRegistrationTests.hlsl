#include "/Test/stf/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_SingleThreadDispatched_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, DispatchThreadId, stf::detail::Scratch.ThreadID);
}

[numthreads(1,1,1)]
void GIVEN_SingleThreadPerGroupAnd10Groups_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, DispatchThreadId, stf::detail::Scratch.ThreadID);
}

[numthreads(10,10,10)]
void GIVEN_SingleGroupWithGroupSizeOf10_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, DispatchThreadId, stf::detail::Scratch.ThreadID);
}

[numthreads(2,2,2)]
void GIVEN_GroupWithSide2WithGroupSizeOfSide2_WHEN_DispatchThreadIdRegistered_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);

    ASSERT(AreEqual, DispatchThreadId, stf::detail::Scratch.ThreadID);
}

[numthreads(1,1,1)]
void GIVEN_ThreadIdNotRegistered_WHEN_ThreadIDQueried_THEN_RegisteredThreadIdIsCorrect(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 expectedData = uint3(0, 0, 0);
    stf::detail::Scratch.Init();

    ASSERT(AreEqual, expectedData, stf::detail::Scratch.ThreadID);
}
