#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/static_assert.hlsli"

STATIC_ASSERT(
    ttl::size_of<ShaderTestPrivate::HLSLAssertMetaData>::value == 24u, 
    "The size of this struct must match the size of HLSLAssertMetaData in TestDataBufferProcessor.h");

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 4u;

    ASSERT(AreEqual, 0u, ShaderTestPrivate::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 0u, ShaderTestPrivate::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 0u, ShaderTestPrivate::Asserts.Num());

    uint resourceSize = 0;
    ShaderTestPrivate::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData);

    ASSERT(AreEqual, bufferSize, ShaderTestPrivate::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 0u, ShaderTestPrivate::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, ShaderTestPrivate::Asserts.Num());

    uint resourceSize = 0;
    ShaderTestPrivate::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAnd200BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{

    const uint bufferSize = 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData) + 200;

    ASSERT(AreEqual, bufferSize, ShaderTestPrivate::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 200u, ShaderTestPrivate::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, ShaderTestPrivate::Asserts.Num());

    uint resourceSize = 0;
    ShaderTestPrivate::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNonMultipleOf8BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint requestedByteSize = 97;
    const uint expectedByteSize = 104;
    const uint bufferSize = 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData) + expectedByteSize;

    ASSERT(AreEqual, bufferSize, ShaderTestPrivate::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, expectedByteSize, ShaderTestPrivate::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, ShaderTestPrivate::Asserts.Num());

    uint resourceSize = 0;
    ShaderTestPrivate::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}