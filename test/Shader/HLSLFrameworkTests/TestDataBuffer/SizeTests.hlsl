#include "/Test/STF/ShaderTestFramework.hlsli"

_Static_assert(
    ttl::size_of<stf::detail::HLSLAssertMetaData>::value == 32u, 
    "The size of this struct must match the size of HLSLAssertMetaData in TestDataBufferProcessor.h");

[numthreads(1,1,1)]
void GIVEN_ZeroAssertsRecorded_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 4u;

    ASSERT(AreEqual, 0u, stf::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 0u, stf::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 0u, stf::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNoData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint bufferSize = 5 * sizeof(stf::detail::HLSLAssertMetaData);

    ASSERT(AreEqual, bufferSize, stf::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 0u, stf::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, stf::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAnd200BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{

    const uint bufferSize = 5 * sizeof(stf::detail::HLSLAssertMetaData) + 200;

    ASSERT(AreEqual, bufferSize, stf::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, 200u, stf::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, stf::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}

[numthreads(1,1,1)]
void GIVEN_FiveAssertsRecordedAndNonMultipleOf8BytesOfData_WHEN_Ran_THEN_AssertBufferInfoAsExpected()
{
    const uint requestedByteSize = 97;
    const uint expectedByteSize = 104;
    const uint bufferSize = 5 * sizeof(stf::detail::HLSLAssertMetaData) + expectedByteSize;

    ASSERT(AreEqual, bufferSize, stf::detail::Asserts.SizeInBytesOfSection());
    ASSERT(AreEqual, expectedByteSize, stf::detail::Asserts.SizeInBytesOfData());
    ASSERT(AreEqual, 5u, stf::detail::Asserts.Num());

    uint resourceSize = 0;
    stf::detail::GetTestDataBuffer().GetDimensions(resourceSize);
    ASSERT(AreEqual, bufferSize, resourceSize);
}