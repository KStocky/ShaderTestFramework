#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO()
    {
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO()
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
    SCENARIO()
    {
        static int counter = 1;
        const int numEntered = counter++;
        static int num = 0;

        static const int Section_1Num = 1;
        if (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;
            ShaderTestPrivate::OnLeave();
        }

        STF::AreEqual(1, num);
        STF::AreEqual(1, numEntered);
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO()
    {
        static int counter = 1;
        const int numEntered = counter++;
        static int num1 = 0;
        static int num2 = 0;
        static const int Section_1Num = 1;
        if (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num1;
            ShaderTestPrivate::OnLeave();
        }
        
        static const int Section_2Num = 2;
        if (ShaderTestPrivate::TryEnterSection(Section_2Num))
        {
            ++num2;
            ShaderTestPrivate::OnLeave();
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
    SCENARIO()
    {
        static int counter = 1;
        const int numEntered = counter++;
        static int num1 = 0;
        static int num2 = 0;
        static int num3 = 0;

        static const int Section_1Num = 1;
        if (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num1;
            ShaderTestPrivate::OnLeave();
        }

        static const int Section_2Num = 2;
        if (ShaderTestPrivate::TryEnterSection(Section_2Num))
        {
            ++num2;

            static const int Section_3Num = 3;
            if (ShaderTestPrivate::TryEnterSection(Section_3Num))
            {
                ++num3;
                ShaderTestPrivate::OnLeave();
            }
            ShaderTestPrivate::OnLeave();
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