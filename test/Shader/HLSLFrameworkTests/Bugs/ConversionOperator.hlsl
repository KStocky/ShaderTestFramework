#include "/Test/STF/ShaderTestFramework.hlsli"

struct TestStruct
{
    int Val;

    bool Test()
    {
        return Val == 0;
    }

    operator bool()
    {
        return Test();
    }
};

[numthreads(1,1,1)]
void GIVEN_StaticObject_WHEN_ConversionOperatorCalled_THEN_Fails(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    TestStruct test;
    test.Val = 0;
    const bool result = (bool)test;          
    STF::IsTrue(result);
}