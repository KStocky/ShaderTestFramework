#include "/Test/STF/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_EmptySection_WHEN_RanUsingSingle_THEN_NoAssertMade(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
        SECTION("")
        {
        }
    }
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSection_WHEN_RanUsingSingle_THEN_SectionsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num = 0;
    SCENARIO("")
    {
        SECTION("")
        {
            ++num;
        }
    }
    
    STF::AreEqual(1, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSections_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    SCENARIO("")
    {
        SECTION("")
        {
            ++num1;
        }
        
        SECTION("")
        {
            ++num2;
        }
    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(1, num2);
    
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingSingle_THEN_EachSectionIsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    SCENARIO("")
    {
        SECTION("")
        {
            ++num1;
        }

        SECTION("")
        {
            ++num2;

            SECTION("")
            {
                ++num3;
            }
        }
    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(1, num2);
    STF::AreEqual(1, num3);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_TwoSubSectionsWithTwoNestedSubsections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
    
    SCENARIO("")
    {
        SECTION("")
        {
            ++num1;
        }

        SECTION("")
        {
            ++num2;

            SECTION("")
            {
                ++num3;
            }
        
            SECTION("")
            {
                ++num4;
            }
        }
    }
    
    STF::AreEqual(1, num1);
    STF::AreEqual(2, num2);
    STF::AreEqual(1, num3);
    STF::AreEqual(1, num4);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1, 1, 1)]
void GIVEN_ThreeLevelsDeepSections_WHEN_RanUsingSingle_THEN_ExpectedSubsectionEntryOccurs(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    int num4 = 0;
    int num5 = 0;
    int num6 = 0;
    int num7 = 0;
    int num8 = 0;
    
    SCENARIO("")
    {
        SECTION("")
        {
            ++num1;
        
            SECTION("")
            {
                ++num5;
                
                SECTION("")
                {
                    ++num6;
                }
                
                SECTION("")
                {
                    ++num8;
                }
            }
        
            SECTION("")
            {
                ++num7;
            }
        }

        SECTION("")
        {
            ++num2;

            SECTION("")
            {
                ++num3;
            }
        
            SECTION("")
            {
                ++num4;
            }
        }
    }
    
    STF::AreEqual(3, num1);
    STF::AreEqual(2, num2);
    STF::AreEqual(1, num3);
    STF::AreEqual(1, num4);
    STF::AreEqual(2, num5);
    STF::AreEqual(1, num6);
    STF::AreEqual(1, num7);
    STF::AreEqual(1, num8);
}