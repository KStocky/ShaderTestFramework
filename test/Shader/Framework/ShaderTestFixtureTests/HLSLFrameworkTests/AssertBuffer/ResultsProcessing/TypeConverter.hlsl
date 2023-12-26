#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestTypeWithoutId
{
    uint Value;
};

struct TestTypeWithTypeId1
{
    uint Value;
};

struct TestTypeWithTypeId2
{
    uint Value;
};

namespace ttl
{
    template<typename From>
    struct caster<bool, From, typename enable_if<
        is_same<From, TestTypeWithoutId>::value ||
        is_same<From, TestTypeWithTypeId1>::value ||
        is_same<From, TestTypeWithTypeId2>::value
        >::type>
    {
        static bool cast(From In)
        {
            return In.Value == 0;
        }
    };
}

namespace STF
{
    template<> struct type_id<TestTypeWithTypeId1> : ttl::integral_constant<uint, TEST_TYPE_1>{};
    template<> struct type_id<TestTypeWithTypeId2> : ttl::integral_constant<uint, TEST_TYPE_2>{};

    template<typename T>
    struct ByteWriter
    <T, typename ttl::enable_if<
        ttl::is_same<T, TestTypeWithoutId>::value || 
        ttl::is_same<T, TestTypeWithTypeId1>::value || 
        ttl::is_same<T, TestTypeWithTypeId2>::value
        >::type
    >
    {
        static const bool HasWriter = true;

        static uint BytesRequired(T)
        {
            return sizeof(T);
        }

        template<typename U>
        static void Write(inout ttl::container<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, In.Value);
        }
    };
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedSingleAssert_WHEN_NoTypeId_THEN_HasExpectedResults()
{
    TestTypeWithoutId t;
    t.Value = 12345678u;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedSingleAssert_WHEN_TypeId_THEN_HasExpectedResults()
{
    TestTypeWithTypeId1 t;
    t.Value = 12345678u;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedTwoSingleAsserts_WHEN_FirstNoTypeIdSecondHasTypeId_THEN_HasExpectedResults()
{
    TestTypeWithoutId t;
    TestTypeWithTypeId1 u;
    u.Value = 12345678u;
    t.Value = 87654321u;
    STF::IsTrue(t, 42);
    STF::IsTrue(u, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveSameTypeId_THEN_HasExpectedResults()
{
    TestTypeWithTypeId1 t;
    t.Value = 12345678u;
    STF::IsTrue(t, 42);

    t.Value = 1234u;
    STF::IsTrue(t, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_FailedTwoSingleAsserts_WHEN_BothHaveDifferentTypeId_THEN_HasExpectedResults()
{
    TestTypeWithTypeId1 t;
    TestTypeWithTypeId2 u;

    t.Value = 12345678u;
    u.Value = 87654321u;
    STF::IsTrue(t, 42);
    STF::IsTrue(u, 42);
}