#include "/Test/STF/ShaderTestFramework.hlsli"

[numthreads(1,1,1)]
void GIVEN_IndexIsAlreadyAligned_WHEN_Aligned_THEN_ReturnsSameIndex()
{
    const uint index = 16u;
    const uint alignment = 8u;

    ASSERT(AreEqual, index, ttl::aligned_offset(index, alignment));
}

[numthreads(1,1,1)]
void GIVEN_IndexIsZero_WHEN_Aligned_THEN_ReturnsZero()
{
    const uint index = 0;
    const uint alignment = 8u;

    ASSERT(AreEqual, index, ttl::aligned_offset(index, alignment));
}

[numthreads(1,1,1)]
void GIVEN_IndexIsOneMinusAlignment_WHEN_Aligned_THEN_Alignment()
{
    const uint index = 7;
    const uint alignment = 8u;

    ASSERT(AreEqual, alignment, ttl::aligned_offset(index, alignment));
}

[numthreads(1,1,1)]
void GIVEN_IndexIsNotAligned_WHEN_Aligned_THEN_ReturnsNextMultipleOfAlignment()
{
    const uint index = 78;
    const uint alignment = 8u;
    const uint expectedIndex = 80u;

    ASSERT(AreEqual, expectedIndex, ttl::aligned_offset(index, alignment));
}