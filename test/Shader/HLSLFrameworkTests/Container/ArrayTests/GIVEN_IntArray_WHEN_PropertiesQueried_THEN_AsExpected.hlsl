#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    using expectedElementType = int;
    static const uint expectedSize = 10;
    using expectedType = expectedElementType[expectedSize];

    static const uint expectedSizeInBytes = expectedSize * sizeof(int);
    using spanType = ttl::container_wrapper<expectedType>;
    spanType span;

    ASSERT(AreEqual, expectedSize, span.size());
    ASSERT(AreEqual, expectedSizeInBytes, span.size_in_bytes());

    ASSERT(IsTrue, ttl::is_same<expectedType, spanType::underlying_type>::value);
    ASSERT(IsTrue, ttl::is_same<expectedElementType, spanType::element_type>::value);
    ASSERT(IsTrue, spanType::writable);
}

