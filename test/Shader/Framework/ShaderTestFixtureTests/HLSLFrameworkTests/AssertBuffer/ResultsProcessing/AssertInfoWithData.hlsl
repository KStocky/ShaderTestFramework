#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestType
{
    uint Value;

    bool operator==(TestType In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestType In)
    {
        return In.Value != Value;
    }
};

struct TestTypeWithTypeIdNoWriter
{
    uint Value;

    bool operator==(TestTypeWithTypeIdNoWriter In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestTypeWithTypeIdNoWriter In)
    {
        return In.Value != Value;
    }
};

struct TestTypeWithTypeIdAndWriter
{
    uint Value;

    bool operator==(TestTypeWithTypeIdAndWriter In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestTypeWithTypeIdAndWriter In)
    {
        return In.Value != Value;
    }
};

struct TestTypeWithNoTypeIdAndWriter
{
    uint Value;

    bool operator==(TestTypeWithNoTypeIdAndWriter In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestTypeWithNoTypeIdAndWriter In)
    {
        return In.Value != Value;
    }
};

struct TestTypeLargeWithTypeIdAndWriter
{
    uint Value;
    float4x4 A;
};

struct TestTypeLargeWithNoTypeIdAndWriter
{
    uint Value;
    float4x4 A;
};

namespace STF
{
    template<>
    bool Cast <bool, TestType>(TestType In)
    {
        return In.Value == 0;
    }

    template<>
    bool Cast <bool, TestTypeWithTypeIdNoWriter>(TestTypeWithTypeIdNoWriter In)
    {
        return In.Value == 0;
    }

    template<>
    bool Cast <bool, TestTypeWithTypeIdAndWriter>(TestTypeWithTypeIdAndWriter In)
    {
        return In.Value == 0;
    }

    template<>
    bool Cast <bool, TestTypeWithNoTypeIdAndWriter>(TestTypeWithNoTypeIdAndWriter In)
    {
        return In.Value == 0;
    }

    template<>
    bool Cast <bool, TestTypeLargeWithTypeIdAndWriter>(TestTypeLargeWithTypeIdAndWriter In)
    {
        return In.Value == 0;
    }

    template<>
    bool Cast <bool, TestTypeLargeWithNoTypeIdAndWriter>(TestTypeLargeWithNoTypeIdAndWriter In)
    {
        return In.Value == 0;
    }

    template<> struct type_id<TestTypeWithTypeIdNoWriter> : ttl::integral_constant<uint, TEST_TYPE_WITH_WRITER>{};
    template<> struct type_id<TestTypeWithTypeIdAndWriter> : ttl::integral_constant<uint, TEST_TYPE_WITH_WRITER>{};
    template<> struct type_id<TestTypeLargeWithTypeIdAndWriter> : ttl::integral_constant<uint, TEST_TYPE_WITH_WRITER>{};

    template<typename T>
    struct ByteWriter
    <T, typename ttl::enable_if<
        ttl::is_same<T, TestTypeWithTypeIdAndWriter>::value || 
        ttl::is_same<T, TestTypeWithNoTypeIdAndWriter>::value || 
        ttl::is_same<T, TestTypeLargeWithTypeIdAndWriter>::value ||
        ttl::is_same<T, TestTypeLargeWithNoTypeIdAndWriter>::value 
        >::type
    >
    {
        static const bool HasWriter = true;

        static uint BytesRequired(T)
        {
            return sizeof(T);
        }

        template<typename U>
        static void Write(inout container<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, In.Value);
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeWithNoTypeIdAndWriter t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults()
{
    TestTypeWithTypeIdNoWriter t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeWithTypeIdAndWriter t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults()
{
    TestType t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeWithNoTypeIdAndWriter t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults()
{
    TestTypeWithTypeIdNoWriter t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeWithTypeIdAndWriter t;
    t.Value = 34;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults()
{
    TestType t;
    TestType u;
    t.Value = 34;
    u.Value = 12345678;
    STF::AreEqual(t, u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeWithNoTypeIdAndWriter t;
    TestTypeWithNoTypeIdAndWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::AreEqual(t, u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdNoWriter_THEN_HasExpectedResults()
{
    TestTypeWithTypeIdNoWriter t;
    TestTypeWithTypeIdNoWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::AreEqual(t, u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeWithTypeIdAndWriter t;
    TestTypeWithTypeIdAndWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::AreEqual(t, u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeLargeWithNoTypeIdAndWriter t;
    TestTypeWithNoTypeIdAndWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::IsTrue(t, 42);
    STF::IsTrue(u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeLargeWithTypeIdAndWriter t;
    TestTypeWithTypeIdAndWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::IsTrue(t, 42);
    STF::IsTrue(u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeLargeWithNoTypeIdAndWriter t;
    TestTypeWithNoTypeIdAndWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::IsTrue(u, 42);
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeLargeWithTypeIdAndWriter t;
    TestTypeWithTypeIdAndWriter u;
    t.Value = 34;
    u.Value = 12345678;
    STF::IsTrue(u, 42);
    STF::IsTrue(t, 42);
}