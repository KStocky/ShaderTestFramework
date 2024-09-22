#include "/Test/stf/ShaderTestFramework.hlsli"

struct TestTypeWithoutId
{
    uint Value;
};

struct TestTypeWithReaderId1
{
    uint Value;
};

struct TestTypeWithReaderId2
{
    uint Value;
};

namespace ttl
{
    template<typename From>
    struct caster<bool, From, typename enable_if<
        is_same<From, TestTypeWithoutId>::value ||
        is_same<From, TestTypeWithReaderId1>::value ||
        is_same<From, TestTypeWithReaderId2>::value
        >::type>
    {
        static bool cast(From In)
        {
            return In.Value == 0;
        }
    };
}

namespace stf
{
    template<> struct ByteReaderTraits<TestTypeWithReaderId1> : ByteReaderTraitsBase<TEST_READER_1>{};
    template<> struct ByteReaderTraits<TestTypeWithReaderId2> : ByteReaderTraitsBase<TEST_READER_2>{};
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedSingleAssert_WHEN_NoReaderId_THEN_HasExpectedResults()
{
    TestTypeWithoutId t;
    t.Value = 12345678u;
    stf::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedSingleAssert_WHEN_ReaderId_THEN_HasExpectedResults()
{
    TestTypeWithReaderId1 t;
    t.Value = 12345678u;
    stf::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedTwoSingleAsserts_WHEN_FirstNoReaderIdSecondHasReaderId_THEN_HasExpectedResults()
{
    TestTypeWithoutId t;
    TestTypeWithReaderId1 u;
    u.Value = 12345678u;
    t.Value = 87654321u;
    stf::IsTrue(t, 42);
    stf::IsTrue(u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveSameReaderId_THEN_HasExpectedResults()
{
    TestTypeWithReaderId1 t;
    t.Value = 12345678u;
    stf::IsTrue(t, 42);

    t.Value = 1234u;
    stf::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveDifferentReaderId_THEN_HasExpectedResults()
{
    TestTypeWithReaderId1 t;
    TestTypeWithReaderId2 u;

    t.Value = 12345678u;
    u.Value = 87654321u;
    stf::IsTrue(t, 42);
    stf::IsTrue(u, 42);
}