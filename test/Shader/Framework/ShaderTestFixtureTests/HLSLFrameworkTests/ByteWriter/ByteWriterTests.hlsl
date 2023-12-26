#include "/Test/STF/ShaderTestFramework.hlsli"


[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_HasWriterQueried_THEN_True()
{
    STF::IsTrue(STF::ByteWriter<bool>::HasWriter);
    STF::IsTrue(STF::ByteWriter<uint>::HasWriter);
    STF::IsTrue(STF::ByteWriter<uint2>::HasWriter);
    STF::IsTrue(STF::ByteWriter<uint3>::HasWriter);
    STF::IsTrue(STF::ByteWriter<uint4>::HasWriter);
    STF::IsTrue(STF::ByteWriter<int>::HasWriter);
    STF::IsTrue(STF::ByteWriter<int2>::HasWriter);
    STF::IsTrue(STF::ByteWriter<int3>::HasWriter);
    STF::IsTrue(STF::ByteWriter<int4>::HasWriter);
    STF::IsTrue(STF::ByteWriter<float>::HasWriter);
    STF::IsTrue(STF::ByteWriter<float2>::HasWriter);
    STF::IsTrue(STF::ByteWriter<float3>::HasWriter);
    STF::IsTrue(STF::ByteWriter<float4>::HasWriter);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FundamentalType_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned()
{
    STF::AreEqual(4u, STF::ByteWriter<bool>::BytesRequired(false));
    STF::AreEqual(4u, STF::ByteWriter<uint>::BytesRequired(uint(3)));
    STF::AreEqual(8u, STF::ByteWriter<uint2>::BytesRequired(uint2(3, 5)));
    STF::AreEqual(12u, STF::ByteWriter<uint3>::BytesRequired(uint3(1,2,3)));
    STF::AreEqual(16u, STF::ByteWriter<uint4>::BytesRequired(uint4(1,2,3,4)));
    STF::AreEqual(4u, STF::ByteWriter<int>::BytesRequired(int(3)));
    STF::AreEqual(8u, STF::ByteWriter<int2>::BytesRequired(int2(1,2)));
    STF::AreEqual(12u, STF::ByteWriter<int3>::BytesRequired(int3(1,2,3)));
    STF::AreEqual(16u, STF::ByteWriter<int4>::BytesRequired(int4(1,2,3,4)));
    STF::AreEqual(4u, STF::ByteWriter<float>::BytesRequired(3.4f));
    STF::AreEqual(8u, STF::ByteWriter<float2>::BytesRequired(float2(1.0f, 2.0f)));
    STF::AreEqual(12u, STF::ByteWriter<float3>::BytesRequired(float3(1.0f, 2.0f, 3.0f)));
    STF::AreEqual(16u, STF::ByteWriter<float4>::BytesRequired(float4(1.0f, 2.0f, 3.0f, 4.0f)));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndBool_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    STF::ByteWriter<bool>::Write(buffer, 0u, true);
    STF::AreEqual(1u, buffer.Data[0]);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    STF::ByteWriter<int>::Write(buffer, 0u, 42);
    STF::AreEqual(42, asint(buffer.Data[0]));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const int2 expectedVal = int2(42, 53);
    STF::ByteWriter<int2>::Write(buffer, 0u, expectedVal);
    const int2 actualVal = int2(asint(buffer.Data[0]), asint(buffer.Data[1]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const int3 expectedVal = int3(42, 53, 98);
    STF::ByteWriter<int3>::Write(buffer, 0u, expectedVal);
    const int3 actualVal = int3(asint(buffer.Data[0]), asint(buffer.Data[1]), asint(buffer.Data[2]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const int4 expectedVal = int4(42, 53, 98, 32);
    STF::ByteWriter<int4>::Write(buffer, 0u, expectedVal);
    const int4 actualVal = int4(asint(buffer.Data[0]), asint(buffer.Data[1]), asint(buffer.Data[2]), asint(buffer.Data[3]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    STF::ByteWriter<uint>::Write(buffer, 0u, 42u);
    STF::AreEqual(42u, asuint(buffer.Data[0]));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const uint2 expectedVal = uint2(42u, 53u);
    STF::ByteWriter<uint2>::Write(buffer, 0u, expectedVal);
    const uint2 actualVal = int2(asuint(buffer.Data[0]), asuint(buffer.Data[1]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const uint3 expectedVal = uint3(42u, 53u, 98u);
    STF::ByteWriter<uint3>::Write(buffer, 0u, expectedVal);
    const uint3 actualVal = uint3(asuint(buffer.Data[0]), asuint(buffer.Data[1]), asuint(buffer.Data[2]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndUInt4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const uint4 expectedVal = uint4(42u, 53u, 98u, 32u);
    STF::ByteWriter<uint4>::Write(buffer, 0u, expectedVal);
    const uint4 actualVal = uint4(asuint(buffer.Data[0]), asuint(buffer.Data[1]), asuint(buffer.Data[2]), asuint(buffer.Data[3]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    STF::ByteWriter<float>::Write(buffer, 0u, 42.0f);
    STF::AreEqual(42.0f, asfloat(buffer.Data[0]));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat2_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const float2 expectedVal = float2(42.0f, 2.0f);
    STF::ByteWriter<float2>::Write(buffer, 0u, expectedVal);
    const float2 actualVal = int2(asfloat(buffer.Data[0]), asfloat(buffer.Data[1]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat3_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const float3 expectedVal = float3(42.0f, 2.0f, -0.5f);
    STF::ByteWriter<float3>::Write(buffer, 0u, expectedVal);
    const float3 actualVal = float3(asfloat(buffer.Data[0]), asfloat(buffer.Data[1]), asfloat(buffer.Data[2]));
    STF::AreEqual(expectedVal, actualVal);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UIntBufferAndFloat4_WHEN_WriteCalled_THEN_BytesSuccessfullyWritten()
{
    ttl::container<uint[4]> buffer;
    const float4 expectedVal = float4(42.0f, 2.0f, -0.5f, 12.5f);
    STF::ByteWriter<float4>::Write(buffer, 0u, expectedVal);
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
    STF::IsFalse(STF::ByteWriter<NoWriter>::HasWriter);
}

struct StructWithWriter
{
    int a;
};

namespace STF
{
    template<>
    struct ByteWriter<StructWithWriter>
    {
        static const bool HasWriter = true;
        static uint BytesRequired(StructWithWriter)
        {
            return sizeof(StructWithWriter);
        }

        template<typename T>
        void Write(ttl::container<T> InContainer, const uint InIndex, const StructWithWriter In)
        {
            InContainer.store(InIndex, In.a);
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonFundamentalTypeWithWriter_WHEN_HasWriterQueried_THEN_True()
{
    STF::IsTrue(STF::ByteWriter<StructWithWriter>::HasWriter);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonFundamentalTypeWithWriter_WHEN_BytesRequiredQueried_THEN_ExpectedNumberReturned()
{
    StructWithWriter test;
    test.a = 42;
    STF::AreEqual(4u, STF::ByteWriter<StructWithWriter>::BytesRequired(test));
}