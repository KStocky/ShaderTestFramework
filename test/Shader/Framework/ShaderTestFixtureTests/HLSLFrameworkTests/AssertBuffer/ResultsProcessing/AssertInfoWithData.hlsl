#include "/Test/Public/ShaderTestFramework.hlsli"

struct TestType
{
    uint Value;
};

struct TestTypeWithTypeIdNoWriter
{
    uint Value;
};

struct TestTypeWithTypeIdAndWriter
{
    uint Value;
};

struct TestTypeWithNoTypeIdAndWriter
{
    uint Value;
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

    template<> struct type_id<TestTypeWithTypeIdNoWriter> : integral_constant<uint, TEST_TYPE_WITH_WRITER>{};
    template<> struct type_id<TestTypeWithTypeIdAndWriter> : integral_constant<uint, TEST_TYPE_WITH_WRITER>{};

    template<typename T>
    struct ByteWriter<T, typename enable_if<is_same<T, TestTypeWithTypeIdAndWriter>::value || is_same<T, TestTypeWithNoTypeIdAndWriter>::value>::type>
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