#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_HasWriterQueried_THEN_False()
{
    STF::IsFalse(ttl::byte_writer<bool>::has_writer);
    STF::IsFalse(ttl::byte_writer<uint>::has_writer);
    STF::IsFalse(ttl::byte_writer<uint2>::has_writer);
    STF::IsFalse(ttl::byte_writer<uint3>::has_writer);
    STF::IsFalse(ttl::byte_writer<uint4>::has_writer);
    STF::IsFalse(ttl::byte_writer<int>::has_writer);
    STF::IsFalse(ttl::byte_writer<int2>::has_writer);
    STF::IsFalse(ttl::byte_writer<int3>::has_writer);
    STF::IsFalse(ttl::byte_writer<int4>::has_writer);
    STF::IsFalse(ttl::byte_writer<float>::has_writer);
    STF::IsFalse(ttl::byte_writer<float2>::has_writer);
    STF::IsFalse(ttl::byte_writer<float3>::has_writer);
    STF::IsFalse(ttl::byte_writer<float4>::has_writer);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned()
{
    STF::AreEqual(4u, ttl::bytes_required(false));
    STF::AreEqual(4u, ttl::bytes_required(uint(3)));
    STF::AreEqual(8u, ttl::bytes_required(uint2(3, 5)));
    STF::AreEqual(12u, ttl::bytes_required(uint3(1,2,3)));
    STF::AreEqual(16u, ttl::bytes_required(uint4(1,2,3,4)));
    STF::AreEqual(4u, ttl::bytes_required(int(3)));
    STF::AreEqual(8u, ttl::bytes_required(int2(1,2)));
    STF::AreEqual(12u, ttl::bytes_required(int3(1,2,3)));
    STF::AreEqual(16u, ttl::bytes_required(int4(1,2,3,4)));
    STF::AreEqual(4u, ttl::bytes_required(3.4f));
    STF::AreEqual(8u, ttl::bytes_required(float2(1.0f, 2.0f)));
    STF::AreEqual(12u, ttl::bytes_required(float3(1.0f, 2.0f, 3.0f)));
    STF::AreEqual(16u, ttl::bytes_required(float4(1.0l, 2.0l, 3.0l, 4.0l)));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_AlignmentRequiredQueried_THEN_ExpectedNumberReturned()
{
    STF::AreEqual(4u, ttl::alignment_required(false));
    STF::AreEqual(4u, ttl::alignment_required(uint(3)));
    STF::AreEqual(4u, ttl::alignment_required(uint2(3, 5)));
    STF::AreEqual(4u, ttl::alignment_required(uint3(1,2,3)));
    STF::AreEqual(4u, ttl::alignment_required(uint4(1,2,3,4)));
    STF::AreEqual(4u, ttl::alignment_required(int(3)));
    STF::AreEqual(4u, ttl::alignment_required(int2(1,2)));
    STF::AreEqual(4u, ttl::alignment_required(int3(1,2,3)));
    STF::AreEqual(4u, ttl::alignment_required(int4(1,2,3,4)));
    STF::AreEqual(4u, ttl::alignment_required(3.4f));
    STF::AreEqual(4u, ttl::alignment_required(float2(1.0f, 2.0f)));
    STF::AreEqual(4u, ttl::alignment_required(float3(1.0f, 2.0f, 3.0f)));
    STF::AreEqual(4u, ttl::alignment_required(float4(1.0f, 2.0f, 3.0f, 4.0f)));
    STF::AreEqual(2u, ttl::alignment_required(float16_t4(1.0f, 2.0f, 3.0f, 4.0f)));
    STF::AreEqual(8u, ttl::alignment_required(float64_t4(1.0l, 2.0l, 3.0l, 4.0l)));
}

struct NoWriter
{
    int a;
    int64_t b;
    float16_t c;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TypeWithNoWriter_WHEN_HasWriterQueried_THEN_False()
{
    STF::IsFalse(ttl::byte_writer<NoWriter>::has_writer);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TypeWithNoWriter_WHEN_BytesRequiredQueried_THEN_ReturnsSizeOfStruct()
{
    NoWriter test;
    STF::AreEqual((uint)sizeof(NoWriter), ttl::bytes_required(test));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TypeWithNoWriter_WHEN_AlignmentRequiredQueried_THEN_ExpectedNumberReturned()
{
    NoWriter test;
    STF::AreEqual(8u, ttl::alignment_required(test));
}

struct StructWithWriter
{
    int a;
};

namespace ttl
{
    template<>
    struct byte_writer<StructWithWriter>
    {
        static const bool has_writer = true;
        static uint bytes_required(StructWithWriter)
        {
            return sizeof(StructWithWriter);
        }

        static uint alignment_required(StructWithWriter)
        {
            return 16u;
        }

        template<typename T>
        static void write(inout container_wrapper<T> InContainer, const uint InIndex, const StructWithWriter In)
        {
            InContainer.store(InIndex, asuint(In.a));
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TypeWithWriter_WHEN_HasWriterQueried_THEN_True()
{
    STF::IsTrue(ttl::byte_writer<StructWithWriter>::has_writer);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TypeWithWriter_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned()
{
    StructWithWriter test;
    test.a = 42;
    STF::AreEqual(4u, ttl::bytes_required(test));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TypeWithWriter_WHEN_AlignmentRequiredQueried_THEN_ExpectedNumberReturned()
{
    StructWithWriter test;
    test.a = 42;
    STF::AreEqual(16u, ttl::alignment_required(test));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndTypeWithWriter_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    uint buffer[4];
    StructWithWriter expectedVal;
    expectedVal.a = 42;
    ttl::write_bytes(buffer, 0u, expectedVal);
    StructWithWriter actualVal;
    actualVal.a = asint(buffer[0]);
    STF::AreEqual(expectedVal.a, actualVal.a);
}