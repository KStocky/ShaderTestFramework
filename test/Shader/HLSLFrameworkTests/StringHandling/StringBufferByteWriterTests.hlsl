#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/StringHandling.hlsli"

using ShaderTestPrivate::StringBuffer;

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyStringBuffer_WHEN_BytesRequiredQueried_THEN_ZeroReturned()
{
    StringBuffer buff = ttl::zero<StringBuffer>();

    ASSERT(AreEqual, ttl::bytes_required(buff), 0u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringBufferWithMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsSameAsNumberOfChars()
{
    StringBuffer buff = ttl::zero<StringBuffer>();

    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);

    ASSERT(AreEqual, ttl::bytes_required(buff), buff.Size);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringBufferWithNonMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsNextMultipleOf4()
{
    StringBuffer buff = ttl::zero<StringBuffer>();

    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);

    ASSERT(AreEqual, ttl::bytes_required(buff), ttl::aligned_offset(buff.Size, 4u));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringBuffer_WHEN_AlignmentRequiredQueried_THEN_FourReturned()
{
    StringBuffer buff = ttl::zero<StringBuffer>();

    ASSERT(AreEqual, ttl::alignment_required(buff), 4u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyStringBuffer_WHEN_WriteCalled_THEN_NothingWritten()
{
    const uint expectedVal = 42;
    uint vec[20];
    ttl::zero(vec);

    vec[0] = 42;
    StringBuffer buff = ttl::zero<StringBuffer>();

    ttl::write_bytes(vec, 0, buff);

    ASSERT(AreEqual, vec[0], expectedVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringBufferWith4Characters_WHEN_WriteCalled_THEN_4CharactersWritten()
{
    const uint expectedVal = 0x01010101;
    uint vec[20];
    ttl::zero(vec);

    StringBuffer buff = ttl::zero<StringBuffer>();
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);

    ttl::write_bytes(vec, 0, buff);

    ASSERT(AreEqual, vec[0], expectedVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringBufferWith5Characters_WHEN_WriteCalled_THEN_5CharactersWritten()
{
    const uint expectedFirstVal = 0x01010101;
    const uint expectedSecondVal = 0x4A;
    uint vec[20];
    ttl::zero(vec);

    StringBuffer buff = ttl::zero<StringBuffer>();
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(1);
    buff.AppendChar(expectedSecondVal);

    ttl::write_bytes(vec, 0, buff);

    ASSERT(AreEqual, vec[0], expectedFirstVal);
    ASSERT(AreEqual, vec[1], expectedSecondVal);
}
