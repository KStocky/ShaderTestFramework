#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ZeroSectionsWithStrings_WHEN_ZeroAssertsFailed_THEN_HasExpectedResults()
{
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ZeroSectionsWithStrings_WHEN_AssertsFailed_THEN_HasExpectedResults()
{
    ASSERT(AreEqual, 42, 34);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_OneScenario_WHEN_AssertsFailedInScenario_THEN_HasExpectedResults()
{
    SCENARIO("TestScenario")
    {
        ASSERT(AreEqual, 42, 34);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_OneSection_WHEN_AssertsFailedInSection_THEN_HasExpectedResults()
{
    SCENARIO("TestScenario")
    {
        SECTION("TestSection")
        {
            ASSERT(AreEqual, 42, 34);
        }
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_OneSection_WHEN_AssertsFailedOutsideSection_THEN_HasExpectedResults()
{
    SCENARIO("TestScenario")
    {
        SECTION("TestSection")
        {
        }

        ASSERT(AreEqual, 42, 34);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNonNestedSection_WHEN_AssertsFailedInFirstSection_THEN_HasExpectedResults()
{
    SCENARIO("TestScenario")
    {
        SECTION("TestSection1")
        {
            ASSERT(AreEqual, 42, 34);
        }

        SECTION("TestSection2")
        {
        }
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoNonNestedSection_WHEN_AssertsFailedInSecondSection_THEN_HasExpectedResults()
{
    SCENARIO("TestScenario")
    {
        SECTION("TestSection1")
        {
        }

        SECTION("TestSection2")
        {
            ASSERT(AreEqual, 42, 34);
        }
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_NestedSections_WHEN_AssertsInEachSection_THEN_HasExpectedResults()
{
    SCENARIO("TestScenario")
    {
        SECTION("TestSection1")
        {
            ASSERT(AreEqual, 42, 34);

            SECTION("TestSection2")
            {
                ASSERT(AreEqual, 42, 34);
            }

            SECTION("TestSection3")
            {
                ASSERT(AreEqual, 42, 34);
            }
        }

        SECTION("TestSection4")
        {
            ASSERT(AreEqual, 42, 34);
        }
    }
}