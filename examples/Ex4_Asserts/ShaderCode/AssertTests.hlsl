// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"

void PassingStandardAsserts()
{
    STF::IsTrue(true);
    STF::IsFalse(false);
    STF::AreEqual(42, 42);
    STF::NotEqual(24, 42);
}

void FailingStandardAsserts()
{
    STF::IsTrue(false);
    STF::IsFalse(true);
    STF::AreEqual(42, 24);
    STF::NotEqual(42, 42);
}

void PassingAssertsWithMacro()
{
    ASSERT(IsTrue, true);
    ASSERT(IsFalse, false);
    ASSERT(AreEqual, 42, 42);
    ASSERT(NotEqual, 24, 42);
}

void FailingAssertsWithMacro()
{
    ASSERT(IsTrue, false);
    ASSERT(IsFalse, true);
    ASSERT(AreEqual, 42, 24);
    ASSERT(NotEqual, 42, 42);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void AssertTests()
{
    PassingStandardAsserts();
    //FailingStandardAsserts();
    PassingAssertsWithMacro();
    //FailingAssertsWithMacro();
}
