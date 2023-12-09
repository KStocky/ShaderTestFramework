#include "/Test/Public/ShaderTestFramework.hlsli"


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
        void Write(STF::container<T> InContainer, const uint InIndex, const StructWithWriter In)
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