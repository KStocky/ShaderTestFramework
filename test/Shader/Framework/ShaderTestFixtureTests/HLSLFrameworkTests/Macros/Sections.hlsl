#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptySection_WHEN_Ran_THEN_NoAssertMade(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO()
    {
        BEGIN_SECTION
        END_SECTION
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num = 0;
    SCENARIO()
    {
        BEGIN_SECTION
            ++num;
        END_SECTION
    }
    
    STF::AreEqual(1, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSections_WHEN_Ran_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    SCENARIO()
    {
        BEGIN_SECTION
            ++num1;
        END_SECTION
        
        BEGIN_SECTION
            ++num2;
        END_SECTION
    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(1, num2);
    
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_Ran_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    SCENARIO()
    {
        BEGIN_SECTION
            ++num1;
        END_SECTION

        BEGIN_SECTION

            ++num2;

            BEGIN_SECTION
                ++num3;
            END_SECTION
        END_SECTION
    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(1, num2);
    STF::AreEqual(1, num3);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_Ran_THEN_ExpectedSubsectionEntryOccurs(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
    
    SCENARIO()
    {
        BEGIN_SECTION
            ++num1;
        END_SECTION

        BEGIN_SECTION

            ++num2;

            BEGIN_SECTION
                ++num3;
            END_SECTION
        
            BEGIN_SECTION
                ++num4;
            END_SECTION
        END_SECTION

    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(2, num2);
    STF::AreEqual(1, num3);
    STF::AreEqual(1, num4);
}