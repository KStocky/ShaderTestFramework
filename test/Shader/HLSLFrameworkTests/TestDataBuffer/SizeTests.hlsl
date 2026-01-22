#include "/Test/STF/ShaderTestFramework.hlsli"

_Static_assert(
    sizeof(stf::AssertionsV1::detail::HLSLAssertMetaData) == 32u, 
    "The size of this struct must match the size of HLSLAssertMetaData in TestDataBufferProcessor.h");

[numthreads(1,1,1)]
void GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 4u;

    ASSERT(AreEqual, 0u, stf::AssertionsV1::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 0u, stf::AssertionsV1::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 0u, stf::AssertionsV1::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::AssertionsV1::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 5 * sizeof(stf::AssertionsV1::detail::HLSLAssertMetaData);

    ASSERT(AreEqual, bufferSize, stf::AssertionsV1::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 0u, stf::AssertionsV1::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, stf::AssertionsV1::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::AssertionsV1::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAnd200BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{

    const uint bufferSize = 5 * sizeof(stf::AssertionsV1::detail::HLSLAssertMetaData) + 200;

    ASSERT(AreEqual, bufferSize, stf::AssertionsV1::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 200u, stf::AssertionsV1::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, stf::AssertionsV1::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::AssertionsV1::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNonMultipleOf8BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint requestedByteSize = 97;
    const uint expectedByteSize = 104;
    const uint bufferSize = 5 * sizeof(stf::AssertionsV1::detail::HLSLAssertMetaData) + expectedByteSize;

    ASSERT(AreEqual, bufferSize, stf::AssertionsV1::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, expectedByteSize, stf::AssertionsV1::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, stf::AssertionsV1::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::AssertionsV1::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}