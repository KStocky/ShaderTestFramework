#include "/Test/STF/ShaderTestFramework.hlsli"

struct EmptyCallable
{
    void operator()(int, int)
    {
    }
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
        static int counter = 0;
        const int num = counter++;
        STF::AreEqual(num, 0);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    SCENARIO("")
    {
        static int counter = 1;
        const int numEntered = counter++;
        static int num = 0;

        static const int Section_1Num = 1;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
            ShaderTestPrivate::Scratch.OnLeave();
        }

        STF::AreEqual(1, num);
        STF::AreEqual(1, numEntered);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    SCENARIO("")
    {
        static int counter = 1;
        const int numEntered = counter++;
        static int num1 = 0;
        static int num2 = 0;
        static const int Section_1Num = 1;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num1;
            ShaderTestPrivate::Scratch.OnLeave();
        }
        
        static const int Section_2Num = 2;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num2;
            ShaderTestPrivate::Scratch.OnLeave();
        }

        if (numEntered == 1)
        {
            STF::AreEqual(1, num1);
            STF::AreEqual(0, num2);
        }
        else if (numEntered == 2)
        {
            STF::AreEqual(1, num1);
            STF::AreEqual(1, num2);
        }
        else
        {
            STF::Fail();
        }
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    SCENARIO("")
    {
        static int counter = 1;
        const int numEntered = counter++;
        static int num1 = 0;
        static int num2 = 0;
        static int num3 = 0;

        static const int Section_1Num = 1;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num1;
            ShaderTestPrivate::Scratch.OnLeave();
        }

        static const int Section_2Num = 2;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num2;

            static const int Section_3Num = 3;
            if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_3Num))
            {
                ++num3;
                ShaderTestPrivate::Scratch.OnLeave();
            }
            ShaderTestPrivate::Scratch.OnLeave();
        }
    
        if (numEntered == 1)
        {
            STF::AreEqual(1, num1);
            STF::AreEqual(0, num2);
            STF::AreEqual(0, num3);
        }
        else if (numEntered == 2)
        {
            STF::AreEqual(1, num1);
            STF::AreEqual(1, num2);
            STF::AreEqual(1, num3);
        }
        else
        {
            STF::Fail();
        }
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ScenarioWithoutId_WHEN_Ran_THEN_IdIsNone(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
    }
    STF::AreEqual(0u, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::None, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(42,1,1)]
void GIVEN_ScenarioWithDispatchThreadId_WHEN_Ran_THEN_IdIsInt3(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    STF::RegisterThreadID(DispatchThreadId);
    SCENARIO("")
    {
    }
    STF::AreEqual(ShaderTestPrivate::FlattenIndex(DispatchThreadId, ShaderTestPrivate::DispatchDimensions), ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int3, ShaderTestPrivate::Scratch.ThreadID.Type);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(42,1,1)]
void GIVEN_ScenarioWithIntId_WHEN_Ran_THEN_IdIsInt()
{
    STF::RegisterThreadID(42);
    SCENARIO("")
    {
    }
    STF::AreEqual(42u, ShaderTestPrivate::Scratch.ThreadID.Data);
    STF::AreEqual(ShaderTestPrivate::EThreadIDType::Int, ShaderTestPrivate::Scratch.ThreadID.Type);
}