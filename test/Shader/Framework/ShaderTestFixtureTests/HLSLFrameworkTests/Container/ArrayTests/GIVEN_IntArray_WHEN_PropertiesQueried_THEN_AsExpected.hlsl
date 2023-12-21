#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_IntArray_WHEN_PropertiesQueried_THEN_AsExpected(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    using expectedElementType = int;
    static const uint expectedSize = 10;
    using expectedType = expectedElementType[expectedSize];

    static const uint expectedSizeInBytes = expectedSize * sizeof(int);
    using spanType = STF::container<expectedType>;
    spanType span;

    STF::AreEqual(expectedSize, span.size());
    STF::AreEqual(expectedSizeInBytes, span.size_in_bytes());

    STF::IsTrue(STF::is_same<expectedType, spanType::underlying_type>::value);
    STF::IsTrue(STF::is_same<expectedElementType, spanType::element_type>::value);
    STF::IsTrue(spanType::writable);
}

