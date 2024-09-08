// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/stf/ShaderTestFramework.hlsli"

void PassingStandardAsserts()
{
    stf::IsTrue(true);
    stf::IsFalse(false);
    stf::AreEqual(42, 42);
    stf::NotEqual(24, 42);
}

void FailingStandardAsserts()
{
    stf::IsTrue(false);
    stf::IsFalse(true);
    stf::AreEqual(42, 24);
    stf::NotEqual(42, 42);
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
