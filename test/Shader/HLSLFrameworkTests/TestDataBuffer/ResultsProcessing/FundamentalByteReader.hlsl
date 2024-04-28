#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoBools_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, true, false);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoScalarUnsignedIntegers_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, 32u, 42u);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoScalarNegativeIntegers_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, -32, -42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoScalarFloatingPoint_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, 32.5, 42.5);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoScalarUnsignedIntegers16bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, uint16_t(32u), uint16_t(42u));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoScalarNegativeIntegers16bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, int16_t(-32), int16_t(-42));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoScalarFloatingPoint16bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, float16_t(32.5), float16_t(42.5));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoVector3FloatingPoint64bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, float64_t3(32.5, 32.5, 32.5), float64_t3(42.5, 42.5, 42.5));
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoMatrix2x2Integer64bit_WHEN_Failed_THEN_OutputHasExpectedSubstrings()
{
    ASSERT(AreEqual, int64_t2x2(32, 32, 32, 32), int64_t2x2(42, 42, 42, 42));
}