#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
SCENARIO(GIVEN_EmptySection_WHEN_Ran_THEN_NoAssertMade, uint3 DispatchThreadId : SV_DispatchThreadID)
{
    BEGIN_SECTION
    END_SECTION
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
SCENARIO(GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce, uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static int counter = 1;
    const int numEntered = counter++;
    static int num = 0;

    BEGIN_SECTION
        ++num;
    END_SECTION

    STF::AreEqual(1, num);
    STF::AreEqual(1, numEntered);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
SCENARIO(GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce, uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static int counter = 1;
    const int numEntered = counter++;
    static int num1 = 0;
    static int num2 = 0;

    BEGIN_SECTION
        ++num1;
    END_SECTION
        
    BEGIN_SECTION
        ++num2;
    END_SECTION

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

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
SCENARIO(GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce, uint3 DispatchThreadId : SV_DispatchThreadID)
{
    static int counter = 1;
    const int numEntered = counter++;
    static int num1 = 0;
    static int num2 = 0;
    static int num3 = 0;

    BEGIN_SECTION
        ++num1;
    END_SECTION

    BEGIN_SECTION
        ++num2;

        BEGIN_SECTION
            ++num3;
        END_SECTION
    END_SECTION
    
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