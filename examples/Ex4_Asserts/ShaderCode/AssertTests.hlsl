// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/stf/ShaderTestFramework.hlsli"

void PassingStandardAsserts()
{
    stf::AssertionsV1::IsTrue(true);
    stf::AssertionsV1::IsFalse(false);
    stf::AssertionsV1::AreEqual(42, 42);
    stf::AssertionsV1::NotEqual(24, 42);
}

void FailingStandardAsserts()
{
    stf::AssertionsV1::IsTrue(false);
    stf::AssertionsV1::IsFalse(true);
    stf::AssertionsV1::AreEqual(42, 24);
    stf::AssertionsV1::NotEqual(42, 42);
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

[numthreads(1, 1, 1)]
void AssertTests()
{
    PassingStandardAsserts();
    //FailingStandardAsserts();
    PassingAssertsWithMacro();
    //FailingAssertsWithMacro();
}
