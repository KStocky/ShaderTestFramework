#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_HasWriterQueried_THEN_True()
{
    STF::IsTrue(ttl::byte_writer<bool>::has_writer);
    STF::IsTrue(ttl::byte_writer<uint>::has_writer);
    STF::IsTrue(ttl::byte_writer<uint2>::has_writer);
    STF::IsTrue(ttl::byte_writer<uint3>::has_writer);
    STF::IsTrue(ttl::byte_writer<uint4>::has_writer);
    STF::IsTrue(ttl::byte_writer<int>::has_writer);
    STF::IsTrue(ttl::byte_writer<int2>::has_writer);
    STF::IsTrue(ttl::byte_writer<int3>::has_writer);
    STF::IsTrue(ttl::byte_writer<int4>::has_writer);
    STF::IsTrue(ttl::byte_writer<float>::has_writer);
    STF::IsTrue(ttl::byte_writer<float2>::has_writer);
    STF::IsTrue(ttl::byte_writer<float3>::has_writer);
    STF::IsTrue(ttl::byte_writer<float4>::has_writer);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned()
{
    STF::AreEqual(4u, ttl::byte_writer<bool>::bytes_required(false));
    STF::AreEqual(4u, ttl::byte_writer<uint>::bytes_required(uint(3)));
    STF::AreEqual(8u, ttl::byte_writer<uint2>::bytes_required(uint2(3, 5)));
    STF::AreEqual(12u, ttl::byte_writer<uint3>::bytes_required(uint3(1,2,3)));
    STF::AreEqual(16u, ttl::byte_writer<uint4>::bytes_required(uint4(1,2,3,4)));
    STF::AreEqual(4u, ttl::byte_writer<int>::bytes_required(int(3)));
    STF::AreEqual(8u, ttl::byte_writer<int2>::bytes_required(int2(1,2)));
    STF::AreEqual(12u, ttl::byte_writer<int3>::bytes_required(int3(1,2,3)));
    STF::AreEqual(16u, ttl::byte_writer<int4>::bytes_required(int4(1,2,3,4)));
    STF::AreEqual(4u, ttl::byte_writer<float>::bytes_required(3.4f));
    STF::AreEqual(8u, ttl::byte_writer<float2>::bytes_required(float2(1.0f, 2.0f)));
    STF::AreEqual(12u, ttl::byte_writer<float3>::bytes_required(float3(1.0f, 2.0f, 3.0f)));
    STF::AreEqual(16u, ttl::byte_writer<float4>::bytes_required(float4(1.0f, 2.0f, 3.0f, 4.0f)));

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
    STF::AreEqual(16u, ttl::bytes_required(float4(1.0f, 2.0f, 3.0f, 4.0f)));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndBool_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    ttl::byte_writer<bool>::write(buffer, 0u, true);
    STF::AreEqual(1u, buffer.Data[0]);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    ttl::byte_writer<int>::write(buffer, 0u, 42);
    STF::AreEqual(42, asint(buffer.Data[0]));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const int2 expectedVal = int2(42, 53);
    ttl::byte_writer<int2>::write(buffer, 0u, expectedVal);
    const int2 actualVal = int2(asint(buffer.Data[0]), asint(buffer.Data[1]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const int3 expectedVal = int3(42, 53, 98);
    ttl::byte_writer<int3>::write(buffer, 0u, expectedVal);
    const int3 actualVal = int3(asint(buffer.Data[0]), asint(buffer.Data[1]), asint(buffer.Data[2]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const int4 expectedVal = int4(42, 53, 98, 32);
    ttl::byte_writer<int4>::write(buffer, 0u, expectedVal);
    const int4 actualVal = int4(asint(buffer.Data[0]), asint(buffer.Data[1]), asint(buffer.Data[2]), asint(buffer.Data[3]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    ttl::byte_writer<uint>::write(buffer, 0u, 42u);
    STF::AreEqual(42u, asuint(buffer.Data[0]));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const uint2 expectedVal = uint2(42u, 53u);
    ttl::byte_writer<uint2>::write(buffer, 0u, expectedVal);
    const uint2 actualVal = int2(asuint(buffer.Data[0]), asuint(buffer.Data[1]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const uint3 expectedVal = uint3(42u, 53u, 98u);
    ttl::byte_writer<uint3>::write(buffer, 0u, expectedVal);
    const uint3 actualVal = uint3(asuint(buffer.Data[0]), asuint(buffer.Data[1]), asuint(buffer.Data[2]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const uint4 expectedVal = uint4(42u, 53u, 98u, 32u);
    ttl::byte_writer<uint4>::write(buffer, 0u, expectedVal);
    const uint4 actualVal = uint4(asuint(buffer.Data[0]), asuint(buffer.Data[1]), asuint(buffer.Data[2]), asuint(buffer.Data[3]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    ttl::byte_writer<float>::write(buffer, 0u, 42.0f);
    STF::AreEqual(42.0f, asfloat(buffer.Data[0]));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const float2 expectedVal = float2(42.0f, 2.0f);
    ttl::byte_writer<float2>::write(buffer, 0u, expectedVal);
    const float2 actualVal = int2(asfloat(buffer.Data[0]), asfloat(buffer.Data[1]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const float3 expectedVal = float3(42.0f, 2.0f, -0.5f);
    ttl::byte_writer<float3>::write(buffer, 0u, expectedVal);
    const float3 actualVal = float3(asfloat(buffer.Data[0]), asfloat(buffer.Data[1]), asfloat(buffer.Data[2]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container_wrapper<uint[4]> buffer;
    const float4 expectedVal = float4(42.0f, 2.0f, -0.5f, 12.5f);
    ttl::write_bytes(buffer, 0u, expectedVal);
    const float4 actualVal = float4(asfloat(buffer.Data[0]), asfloat(buffer.Data[1]), asfloat(buffer.Data[2]), asfloat(buffer.Data[3]));
    STF::AreEqual(expectedVal, actualVal);
}

struct NoWriter
{
    int a;
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonFundamentalTypeWithNoWriter_WHEN_HasWriterQueried_THEN_False()
{
    STF::IsFalse(ttl::byte_writer<NoWriter>::has_writer);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonFundamentalTypeWithNoWriter_WHEN_BytesRequiredQueried_THEN_Zero()
{
    NoWriter test;
    STF::AreEqual(0u, ttl::bytes_required(test));
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

        template<typename T>
        void write(container_wrapper<T> InContainer, const uint InIndex, const StructWithWriter In)
        {
            InContainer.store(InIndex, In.a);
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonFundamentalTypeWithWriter_WHEN_HasWriterQueried_THEN_True()
{
    STF::IsTrue(ttl::byte_writer<StructWithWriter>::has_writer);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonFundamentalTypeWithWriter_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned()
{
    StructWithWriter test;
    test.a = 42;
    STF::AreEqual(4u, ttl::byte_writer<StructWithWriter>::bytes_required(test));
}