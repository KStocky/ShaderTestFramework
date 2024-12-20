// We have set "/Shader" to be the virtual file directory for our shader code so we can
// use that to include headers
#include "/Shader/Optional.hlsli"

// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/stf/ShaderTestFramework.hlsli"

void GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid(const Optional<int> InOpt)
{
    ASSERT(IsFalse, InOpt.IsValid);
}

void GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned(const Optional<int> InOpt)
{
    const int expectedValue = 42;

    ASSERT(AreEqual, expectedValue, InOpt.GetOrDefault(expectedValue));
}

void GIVEN_InvalidOptional_WHEN_ValueSet_THEN_IsValid(Optional<int> InOpt)
{
    const int expectedValue = 42;

    InOpt.Set(expectedValue);
    ASSERT(IsTrue, InOpt.IsValid);
}

void GIVEN_InvalidOptional_WHEN_ValueSet_THEN_GetReturnsValue(Optional<int> InOpt)
{
    const int expectedValue = 42;
    const int defaultValue = 24;

    InOpt.Set(expectedValue);
    ASSERT(AreEqual,  expectedValue, InOpt.GetOrDefault(defaultValue));
}

[numthreads(1, 1, 1)]
void OptionalTestsWithoutScenariosAndSections()
{
    Optional<int> opt;
    opt.Reset();

    GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid(opt);
    GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned(opt);
    GIVEN_InvalidOptional_WHEN_ValueSet_THEN_IsValid(opt);
    GIVEN_InvalidOptional_WHEN_ValueSet_THEN_GetReturnsValue(opt);
}

[numthreads(1, 1, 1)]
void OptionalTestsWithScenariosAndSections()
{
    SCENARIO("GIVEN An Optional that is reset")
    {
        Optional<int> opt;
        opt.Reset();

        SECTION("THEN IsValid returns false")
        {
            ASSERT(IsFalse, opt.IsValid);
        }

        SECTION("THEN GetOrDefault returns default value")
        {
            const int expectedValue = 42;
            ASSERT(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION("WHEN value is set")
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION("THEN IsValid returns true")
            {
                ASSERT(IsTrue, opt.IsValid);
            }

            SECTION("THEN GetOrDefault returns set value")
            {
                const int defaultValue = 24;
                ASSERT(AreEqual,  expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}

[numthreads(32, 1, 1)]
void OptionalTestsWithScenariosAndSectionsAndThreadIds(uint3 DTid : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DTid);
    SCENARIO("GIVEN An Optional that is reset")
    {
        Optional<int> opt;
        opt.Reset();

        SECTION("THEN IsValid returns false")
        {
            //if (DTid.x == 16)
            //{
            //    ASSERT(IsTrue, opt.IsValid);
            //}
            //else
            {
                ASSERT(IsFalse, opt.IsValid);
            }
        }

        SECTION("THEN GetOrDefault returns default value")
        {
            const int expectedValue = 42;
            ASSERT(AreEqual, expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION("WHEN value is set")
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION("THEN IsValid returns true")
            {
                ASSERT(IsTrue, opt.IsValid);
            }

            SECTION("THEN GetOrDefault returns set value")
            {
                const int defaultValue = 24;
                ASSERT(AreEqual,  expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}