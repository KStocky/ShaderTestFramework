#include "/Test/STF/ShaderTestFramework.hlsli"

struct UnknownAlignmentLessThan8
{
    int A;
};

struct UnknownAlignmentGreaterThan8
{
    int A;
    int B;
    int C;
};

struct ReportedAlignment
{
    float A;
};

namespace ttl
{
    template<> struct align_of<ReportedAlignment> : integral_constant<uint, 4>{};
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void AllHaveExpectedAlignment()
{
    ASSERT(AreEqual, 2u, ttl::align_of<int16_t>::value);
    ASSERT(AreEqual, 4u, ttl::align_of<int32_t>::value);
    ASSERT(AreEqual, 8u, ttl::align_of<int64_t>::value);
    ASSERT(AreEqual, 4u, ttl::align_of<bool>::value);

    ASSERT(AreEqual, (uint)sizeof(UnknownAlignmentLessThan8), ttl::align_of<UnknownAlignmentLessThan8>::value);
    ASSERT(AreEqual, 8u, ttl::align_of<UnknownAlignmentGreaterThan8>::value);
    ASSERT(AreEqual, 4u, ttl::align_of<ReportedAlignment>::value);
}