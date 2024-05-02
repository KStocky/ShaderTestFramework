#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_BytesRequiredQueried_THEN_ZeroReturned()
{
    ttl::string buff = ttl::zero<ttl::string>();

    ASSERT(AreEqual, ttl::bytes_required(buff), 0u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringWithMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsSameAsNumberOfChars()
{
    ttl::string buff = ttl::zero<ttl::string>();

    buff.append(1);
    buff.append(1);
    buff.append(1);
    buff.append(1);

    ASSERT(AreEqual, ttl::bytes_required(buff), buff.Size);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringWithNonMultipleOf4Characters_WHEN_BytesRequiredQueried_THEN_BytesRequiredIsNextMultipleOf4()
{
    ttl::string buff = ttl::zero<ttl::string>();

    buff.append(1);
    buff.append(1);
    buff.append(1);
    buff.append(1);
    buff.append(1);

    ASSERT(AreEqual, ttl::bytes_required(buff), ttl::aligned_offset(buff.Size, 4u));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_String_WHEN_AlignmentRequiredQueried_THEN_FourReturned()
{
    ttl::string buff = ttl::zero<ttl::string>();

    ASSERT(AreEqual, ttl::alignment_required(buff), 4u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_WriteCalled_THEN_NothingWritten()
{
    const uint expectedVal = 42;
    uint vec[20];
    ttl::zero(vec);

    vec[0] = 42;
    ttl::string buff = ttl::zero<ttl::string>();

    ttl::write_bytes(vec, 0, buff);

    ASSERT(AreEqual, vec[0], expectedVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringWith4Characters_WHEN_WriteCalled_THEN_4CharactersWritten()
{
    const uint expectedVal = 0x01010101;
    uint vec[20];
    ttl::zero(vec);

    ttl::string buff = ttl::zero<ttl::string>();
    buff.append(1);
    buff.append(1);
    buff.append(1);
    buff.append(1);

    ttl::write_bytes(vec, 0, buff);

    ASSERT(AreEqual, vec[0], expectedVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_StringWith5Characters_WHEN_WriteCalled_THEN_5CharactersWritten()
{
    const uint expectedFirstVal = 0x01010101;
    const uint expectedSecondVal = 0x4A;
    uint vec[20];
    ttl::zero(vec);

    ttl::string buff = ttl::zero<ttl::string>();
    buff.append(1);
    buff.append(1);
    buff.append(1);
    buff.append(1);
    buff.append(expectedSecondVal);

    ttl::write_bytes(vec, 0, buff);

    ASSERT(AreEqual, vec[0], expectedFirstVal);
    ASSERT(AreEqual, vec[1], expectedSecondVal);
}
