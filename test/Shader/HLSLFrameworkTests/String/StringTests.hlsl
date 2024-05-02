#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/string.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_CharacterAppended_THEN_DataIsAsExpected()
{
    ttl::string buff = ttl::zero<ttl::string>();
    const uint expectedChar = 0x42;

    buff.append(expectedChar);

    ASSERT(AreEqual, buff.Data[0], expectedChar);
    ASSERT(AreEqual, buff.Size, 1u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_AttemptingToAppendTwoCharactersAtOnce_THEN_OnlyTheFirstIsAppended()
{
    ttl::string buff = ttl::zero<ttl::string>();
    const uint expectedChar = 0x42;
    const uint twoChars = 0x3400 | expectedChar;

    buff.append(twoChars);

    ASSERT(AreEqual, buff.Data[0], expectedChar);
    ASSERT(AreEqual, buff.Size, 1u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_FourCharactersAppended_THEN_DataIsAsExpected()
{
    ttl::string buff = ttl::zero<ttl::string>();
    const uint firstChar = 0x42;
    const uint secondChar = 0x52;
    const uint thirdChar = 0x10;
    const uint fourthChar = 0x23;

    buff.append(firstChar);
    buff.append(secondChar);
    buff.append(thirdChar);
    buff.append(fourthChar);

    const uint expectedValue = 0x23105242;

    ASSERT(AreEqual, buff.Data[0], expectedValue);
    ASSERT(AreEqual, buff.Data[1], 0u);
    ASSERT(AreEqual, buff.Size, 4u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_FiveCharactersAppended_THEN_DataIsAsExpected()
{
    ttl::string buff = ttl::zero<ttl::string>();
    const uint firstChar = 0x42;
    const uint secondChar = 0x52;
    const uint thirdChar = 0x10;
    const uint fourthChar = 0x23;
    const uint fifthChar = 0x2A;

    buff.append(firstChar);
    buff.append(secondChar);
    buff.append(thirdChar);
    buff.append(fourthChar);
    buff.append(fifthChar);

    const uint expectedFirstValue = 0x23105242;
    const uint expectedSecondValue = fifthChar;

    ASSERT(AreEqual, buff.Data[0], expectedFirstValue);
    ASSERT(AreEqual, buff.Data[1], expectedSecondValue);
    ASSERT(AreEqual, buff.Size, 5u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyStringCreator_WHEN_Executed_THEN_ReturnedStringIsEmpty()
{
    DEFINE_STRING_CREATOR(str1, "");
    ttl::string buff = str1();

    ASSERT(AreEqual, buff.Data[0], 0u);
    // Null terminator is counted as character.
    ASSERT(AreEqual, buff.Size, 1u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NonEmptyStringCreator_WHEN_Executed_THEN_ReturnedStringIsAsExpected()
{
    DEFINE_STRING_CREATOR(str1, "Hello");
    ttl::string buff = str1();

    ASSERT(AreEqual, buff.Data[0], 0x6C6C6548u);
    ASSERT(AreEqual, buff.Data[1], 0x6Fu);
    ASSERT(AreEqual, buff.Size, 6u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNonEmptyStringCreator_WHEN_Executed_THEN_BothStringsAreAsExpected()
{
    DEFINE_STRING_CREATOR(str1, "Hello");
    DEFINE_STRING_CREATOR(str2, "Bye");

    ttl::string buff1 = str1();
    ttl::string buff2 = str2();

    ASSERT(AreEqual, buff1.Data[0], 0x6C6C6548u);
    ASSERT(AreEqual, buff1.Data[1], 0x6Fu);
    ASSERT(AreEqual, buff1.Size, 6u);

    ASSERT(AreEqual, buff2.Data[0], 0x657942u);
    ASSERT(AreEqual, buff2.Size, 4u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_FullString_WHEN_AppendCalled_THEN_AppendFails()
{
    DEFINE_STRING_CREATOR(fullString, "123456789012345678901234567890123456789012345678901234567890123");
    ttl::string buff = fullString();

    ASSERT(AreEqual, buff.Size, ttl::string::MaxNumChars);

    buff.append('!');
    ASSERT(AreEqual, buff.Size, ttl::string::MaxNumChars);
}
