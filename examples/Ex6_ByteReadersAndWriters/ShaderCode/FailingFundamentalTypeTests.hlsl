
// "/Test" is the virtual shader directory for the framework
// This is defined by the framework itself.
#include "/Test/STF/ShaderTestFramework.hlsli"


void FailingFundamentalAsserts()
{
    ASSERT(AreEqual, uint16_t(42), uint16_t(24));
    ASSERT(NotEqual, float64_t3(2.0l, 2.0l, 2.0l), float64_t3(2.0l, 2.0l, 2.0l));
    ASSERT(AreEqual, bool2x2(true, true, false, false), bool2x2(false, false, true, true));
}

[numthreads(1, 1, 1)]
void Tests()
{
    //FailingFundamentalAsserts();
}
