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

struct TestTypeLargeWithTypeIdAndWriter
{
    uint Value;
    uint2 A;

    bool operator==(TestTypeLargeWithTypeIdAndWriter In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestTypeLargeWithTypeIdAndWriter In)
    {
        return In.Value != Value;
    }
};

struct TestTypeLargeWithNoTypeIdAndWriter
{
    uint Value;
    uint2 A;

    bool operator==(TestTypeLargeWithNoTypeIdAndWriter In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestTypeLargeWithNoTypeIdAndWriter In)
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

struct TestTypeWithVaryingSize
{
    uint Value;

    bool operator==(TestTypeWithVaryingSize In)
    {
        return In.Value == Value;
    }

    bool operator!=(TestTypeWithVaryingSize In)
    {
        return In.Value != Value;
    }
};

namespace ttl
{
    template<typename From>
    struct caster<bool, From, typename enable_if<
        is_same<From, TestType>::value ||
        is_same<From, TestTypeWithTypeIdNoWriter>::value ||
        is_same<From, TestTypeWithTypeIdAndWriter>::value ||
        is_same<From, TestTypeWithNoTypeIdAndWriter>::value ||
        is_same<From, TestTypeLargeWithTypeIdAndWriter>::value ||
        is_same<From, TestTypeLargeWithNoTypeIdAndWriter>::value ||
        is_same<From, TestTypeWithVaryingSize>::value
        >::type>
    {
        static bool cast(From In)
        {
            return In.Value == 0;
        }
    };

    template<>
    struct byte_writer<TestTypeWithVaryingSize>
    {
        static const bool has_writer = true;

        static uint bytes_required(TestTypeWithVaryingSize In)
        {
            return In.Value * 4;
        }

        template<typename U>
        static void write(inout container_wrapper<U> InContainer, const uint InIndex, const TestTypeWithVaryingSize In)
        {
            for (uint i = 0; i < In.Value; ++i)
            {
                InContainer.store(InIndex + i * 4u, i + 1u);
            }
        }

    };

    template<typename T>
    struct byte_writer<T, typename enable_if<
        is_same<T, TestTypeWithTypeIdAndWriter>::value || 
        is_same<T, TestTypeWithNoTypeIdAndWriter>::value || 
        is_same<T, TestTypeLargeWithTypeIdAndWriter>::value ||
        is_same<T, TestTypeLargeWithNoTypeIdAndWriter>::value 
        >::type
    >
    {
        static const bool has_writer = true;

        static uint bytes_required(T)
        {
            return sizeof(T);
        }

        template<typename U, typename T1>
        static typename enable_if<
        is_same<T1, TestTypeWithTypeIdAndWriter>::value || 
        is_same<T1, TestTypeWithNoTypeIdAndWriter>::value
        >::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T1 In)
        {
            InContainer.store(InIndex, In.Value);
        }

        template<typename U, typename T1>
        static typename enable_if<
        is_same<T1, TestTypeLargeWithTypeIdAndWriter>::value ||
        is_same<T1, TestTypeLargeWithNoTypeIdAndWriter>::value
        >::type write(inout container_wrapper<U> InContainer, const uint InIndex, const T1 In)
        {
            InContainer.store(InIndex, In.Value);
            InContainer.store(InIndex + 4, In.A.x, In.A.y);
        }
    };
}

namespace STF
{
    template<> struct type_id<TestTypeWithTypeIdNoWriter> : ttl::integral_constant<uint, TEST_TYPE_WITH_WRITER>{};
    template<> struct type_id<TestTypeWithTypeIdAndWriter> : ttl::integral_constant<uint, TEST_TYPE_WITH_WRITER>{};
    template<> struct type_id<TestTypeLargeWithTypeIdAndWriter> : ttl::integral_constant<uint, TEST_TYPE_WITH_WRITER>{};
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
    t.A = uint2(123, 42);
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
    t.A = uint2(123, 42);
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
    t.A = uint2(123, 42);
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
    t.A = uint2(123, 42);
    u.Value = 12345678;
    STF::IsTrue(u, 42);
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_SmallStructComparedWithLargerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults()
{
    TestTypeWithVaryingSize t;
    TestTypeWithVaryingSize u;
    t.Value = 1;
    u.Value = 3;
    STF::AreEqual(t, u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_LargeStructComparedWithSmallerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults()
{
    TestTypeWithVaryingSize t;
    TestTypeWithVaryingSize u;
    t.Value = 1;
    u.Value = 3;
    STF::AreEqual(u, t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_OneLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeLargeWithNoTypeIdAndWriter t;
    TestTypeLargeWithNoTypeIdAndWriter u;
    t.Value = 1000;
    t.A = uint2(2000, 3000);
    u.Value = 4000;
    u.A = uint2(5000, 6000);
    STF::AreEqual(t, u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_AssertInfoAndDataCapacity_WHEN_TwoLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults()
{
    TestTypeLargeWithNoTypeIdAndWriter t;
    TestTypeLargeWithNoTypeIdAndWriter u;
    t.Value = 1000;
    t.A = uint2(2000, 3000);
    u.Value = 4000;
    u.A = uint2(5000, 6000);
    STF::AreEqual(t, u, 42);
    STF::AreEqual(t, u, 42);
}