#include "/Test/STF/ShaderTestFramework.hlsli"
#include "/Test/TTL/concepts.hlsli"
#include "/Test/TTL/models.hlsli"
#include "/Test/TTL/string.hlsli"

[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_CharacterAppended_THEN_DataIsAsExpected()
{
    ttl::string<10> buff = ttl::zero<ttl::string<10> >();
    const uint expectedChar = 0x42;

    buff.append(expectedChar);

    ASSERT(AreEqual, buff.Data[0], expectedChar);
    ASSERT(AreEqual, buff.Size, 1u);
}

[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_AttemptingToAppendTwoCharactersAtOnce_THEN_OnlyTheFirstIsAppended()
{
    ttl::string<10> buff = ttl::zero<ttl::string<10> >();
    const uint expectedChar = 0x42;
    const uint twoChars = 0x3400 | expectedChar;

    buff.append(twoChars);

    ASSERT(AreEqual, buff.Data[0], expectedChar);
    ASSERT(AreEqual, buff.Size, 1u);
}

[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_FourCharactersAppended_THEN_DataIsAsExpected()
{
    ttl::string<10> buff = ttl::zero<ttl::string<10> >();
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

[numthreads(1,1,1)]
void GIVEN_EmptyString_WHEN_FiveCharactersAppended_THEN_DataIsAsExpected()
{
    ttl::string<10> buff = ttl::zero<ttl::string<10> >();
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

[numthreads(1,1,1)]
void GIVEN_EmptyStringCreator_WHEN_Executed_THEN_ReturnedStringIsEmpty()
{
    CREATE_STRING(str1, "");

    ASSERT(AreEqual, str1.Data[0], 0u);
    // Null terminator is counted as character.
    ASSERT(AreEqual, str1.Size, 1u);
}

[numthreads(1,1,1)]
void GIVEN_NonEmptyStringCreator_WHEN_Executed_THEN_ReturnedStringIsAsExpected()
{
    CREATE_STRING(str1, "Hello");

    ASSERT(AreEqual, str1.Data[0], 0x6C6C6548u);
    ASSERT(AreEqual, str1.Data[1], 0x6Fu);
    ASSERT(AreEqual, str1.Size, 6u);
}

[numthreads(1,1,1)]
void GIVEN_TwoNonEmptyStringCreator_WHEN_Executed_THEN_BothStringsAreAsExpected()
{
    CREATE_STRING(str1, "Hello");
    CREATE_STRING(str2, "Bye");

    ASSERT(AreEqual, str1.Data[0], 0x6C6C6548u);
    ASSERT(AreEqual, str1.Data[1], 0x6Fu);
    ASSERT(AreEqual, str1.Size, 6u);

    ASSERT(AreEqual, str2.Data[0], 0x657942u);
    ASSERT(AreEqual, str2.Size, 4u);
}

[numthreads(1,1,1)]
void GIVEN_FullString_WHEN_AppendCalled_THEN_AppendFails()
{
    CREATE_STRING(fullString, "FullString");

    ASSERT(AreEqual, fullString.Size, fullString.MaxNumChars);

    fullString.append('!');
    ASSERT(AreEqual, fullString.Size, fullString.MaxNumChars);
}
