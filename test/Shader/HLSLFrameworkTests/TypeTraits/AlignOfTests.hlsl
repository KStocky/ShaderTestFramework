#include "/Test/STF/ShaderTestFramework.hlsli"

struct UnknownAlignment
{
    int A;
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

    ASSERT(AreEqual, 8u, ttl::align_of<UnknownAlignment>::value);
    ASSERT(AreEqual, 4u, ttl::align_of<ReportedAlignment>::value);
}