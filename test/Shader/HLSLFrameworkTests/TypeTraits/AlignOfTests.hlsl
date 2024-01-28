#include "/Test/TTL/static_assert.hlsli"
#include "/Test/TTL/type_traits.hlsli"

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

[numthreads(1,1,1)]
void AllHaveExpectedAlignment()
{
    STATIC_ASSERT(2u == ttl::align_of<int16_t>::value);
    STATIC_ASSERT(4u == ttl::align_of<int32_t>::value);
    STATIC_ASSERT(8u == ttl::align_of<int64_t>::value);
    STATIC_ASSERT(4u == ttl::align_of<bool>::value);

    STATIC_ASSERT(2u == ttl::align_of<int16_t3>::value);
    STATIC_ASSERT(8u == ttl::align_of<int64_t4>::value);

    STATIC_ASSERT(2u == ttl::align_of<int16_t3x4>::value);
    STATIC_ASSERT(8u == ttl::align_of<int64_t4x2>::value);

    STATIC_ASSERT((uint)sizeof(UnknownAlignmentLessThan8) == ttl::align_of<UnknownAlignmentLessThan8>::value);
    STATIC_ASSERT(8u == ttl::align_of<UnknownAlignmentGreaterThan8>::value);
    STATIC_ASSERT(4u == ttl::align_of<ReportedAlignment>::value);
}