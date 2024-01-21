#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 4u;

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
    const uint bufferSize = 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData);

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(0u, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(5u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAnd200BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{

    const uint bufferSize = 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData) + 200;

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(200u, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(5u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNonMultipleOf8BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint requestedByteSize = 97;
    const uint expectedByteSize = 104;
    const uint bufferSize = 5 * sizeof(ShaderTestPrivate::HLSLAssertMetaData) + expectedByteSize;

    STF::AreEqual(bufferSize, ShaderTestPrivate::SizeInBytesOfAssertBuffer);
    STF::AreEqual(requestedByteSize, ShaderTestPrivate::SizeInBytesOfAssertData);
    STF::AreEqual(5u, ShaderTestPrivate::MaxNumAsserts);

    uint resourceSize = 0;
    ShaderTestPrivate::GetAssertBuffer().GetDimensions(resourceSize);
    STF::AreEqual(bufferSize, resourceSize);
}