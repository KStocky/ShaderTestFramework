#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 8u;

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(0u, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(0u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 8u + 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData);

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(0u, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(5u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAnd100BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{

    const uint bufferSize = 8u + 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData) + sizeof(uint) + 100;

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(100u, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(5u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNonMultipleOf4BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint requestedByteSize = 97;
    const uint expectedByteSize = 100;
    const uint bufferSize = 8u + 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData) + sizeof(uint) + expectedByteSize;

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(requestedByteSize, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(5u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}