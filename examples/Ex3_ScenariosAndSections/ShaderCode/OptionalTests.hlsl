// We have set "/Shader" to be the virtual file directory for our shader code so we can
// use that to include headers
#include "/Shader/Optional.hlsli"

// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid()
{
    Optional<int> opt;
    opt.Reset();

    STF::IsFalse(opt.IsValid);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned()
{
    Optional<int> opt;
    opt.Reset();

    const int expectedValue = 42;

    STF::AreEqual(expectedValue, opt.GetOrDefault(expectedValue));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_InvalidOptional_WHEN_ValueSet_THEN_IsValid()
{
    Optional<int> opt;
    opt.Reset();

    const int expectedValue = 42;

    opt.Set(expectedValue);
    STF::IsTrue(opt.IsValid);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_InvalidOptional_WHEN_ValueSet_THEN_GetReturnsValue()
{
    Optional<int> opt;
    opt.Reset();

    const int expectedValue = 42;
    const int defaultValue = 24;

    opt.Set(expectedValue);
    STF::AreEqual( expectedValue, opt.GetOrDefault(defaultValue));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void OptionalTests()
{
    SCENARIO(/*GIVEN An Optional that is reset*/)
    {
        Optional<int> opt;
        opt.Reset();

        SECTION(/*THEN IsValid returns false*/)
        {
            STF::IsFalse(opt.IsValid);
        }

        SECTION(/*THEN GetOrDefault returns default value*/)
        {
            const int expectedValue = 42;
            STF::AreEqual(expectedValue, opt.GetOrDefault(expectedValue));
        }

        SECTION(/*WHEN value is set*/)
        {
            const int expectedValue = 42;
            opt.Set(expectedValue);

            SECTION(/*THEN IsValid returns true*/)
            {
                STF::IsTrue(opt.IsValid);
            }

            SECTION(/*THEN GetOrDefault returns set value*/)
            {
                const int defaultValue = 24;
                STF::AreEqual( expectedValue, opt.GetOrDefault(defaultValue));
            }
        }
    }
}