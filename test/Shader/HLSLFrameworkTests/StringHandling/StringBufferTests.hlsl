#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/STF/StringHandling.hlsli"

using ShaderTestPrivate::StringBuffer;

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyStringBuffer_WHEN_CharacterAppended_THEN_DataIsAsExpected()
{
    StringBuffer buff = ttl::zero<StringBuffer>();
    const uint expectedChar = 0x42;

    buff.AppendChar(expectedChar);

    ASSERT(AreEqual, buff.Data[0], expectedChar);
    ASSERT(AreEqual, buff.Size, 1u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyStringBuffer_WHEN_FourCharactersAppended_THEN_DataIsAsExpected()
{
    StringBuffer buff = ttl::zero<StringBuffer>();
    const uint firstChar = 0x42;
    const uint secondChar = 0x52;
    const uint thirdChar = 0x10;
    const uint fourthChar = 0x23;

    buff.AppendChar(firstChar);
    buff.AppendChar(secondChar);
    buff.AppendChar(thirdChar);
    buff.AppendChar(fourthChar);

    const uint expectedValue = 0x23105242;

    ASSERT(AreEqual, buff.Data[0], expectedValue);
    ASSERT(AreEqual, buff.Data[1], 0u);
    ASSERT(AreEqual, buff.Size, 4u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyStringBuffer_WHEN_FiveCharactersAppended_THEN_DataIsAsExpected()
{
    StringBuffer buff = ttl::zero<StringBuffer>();
    const uint firstChar = 0x42;
    const uint secondChar = 0x52;
    const uint thirdChar = 0x10;
    const uint fourthChar = 0x23;
    const uint fifthChar = 0x2A;

    buff.AppendChar(firstChar);
    buff.AppendChar(secondChar);
    buff.AppendChar(thirdChar);
    buff.AppendChar(fourthChar);
    buff.AppendChar(fifthChar);

    const uint expectedFirstValue = 0x23105242;
    const uint expectedSecondValue = fifthChar;

    ASSERT(AreEqual, buff.Data[0], expectedFirstValue);
    ASSERT(AreEqual, buff.Data[1], expectedSecondValue);
    ASSERT(AreEqual, buff.Size, 5u);
}