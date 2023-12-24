#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;
    }  
    
    STF::AreEqual(1, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSubsection_WHEN_RanUsingIf_THEN_SectionsEntered2Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;
        static const int Section_1Num = 1;
        if (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;

            ShaderTestPrivate::OnLeave();
        }
    }  
    
    STF::AreEqual(2, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered2Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;
        static const int Section_1Num = 1;
        while (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;
        }
    }  
    
    STF::AreEqual(2, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubsections_WHEN_RanUsingIf_THEN_SectionsEntered4Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;
        static const int Section_1Num = 1;
        if (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;
            ShaderTestPrivate::OnLeave();
        }
        
        static const int Section_2Num = 2;
        if (ShaderTestPrivate::TryEnterSection(Section_2Num))
        {
            ++num;
            ShaderTestPrivate::OnLeave();
        }
    }  
    
    STF::AreEqual(4, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubsections_WHEN_RanUsingWhile_THEN_SectionsEntered4Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;
        static const int Section_1Num = 1;
        while (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;
        }
        
        static const int Section_2Num = 2;
        while (ShaderTestPrivate::TryEnterSection(Section_2Num))
        {
            ++num;
        }
    }  
    
    STF::AreEqual(4, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingIf_THEN_SectionsEntered5Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;

        static const int Section_1Num = 1;
        if (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;
            ShaderTestPrivate::OnLeave();
        }

        static const int Section_2Num = 2;
        if (ShaderTestPrivate::TryEnterSection(Section_2Num))
        {
            ++num;

            static const int Section_3Num = 3;
            if (ShaderTestPrivate::TryEnterSection(Section_3Num))
            {
                ++num;
                ShaderTestPrivate::OnLeave();
            }
            ShaderTestPrivate::OnLeave();
        }
    }  
    
    STF::AreEqual(5, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered5Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    ShaderTestPrivate::InitScratch();
    int num = 0;
    while (ShaderTestPrivate::TryLoopScenario())
    {
        ++num;

        static const int Section_1Num = 1;
        while (ShaderTestPrivate::TryEnterSection(Section_1Num))
        {
            ++num;
        }

        static const int Section_2Num = 2;
        while (ShaderTestPrivate::TryEnterSection(Section_2Num))
        {
            ++num;

            static const int Section_3Num = 3;
            while (ShaderTestPrivate::TryEnterSection(Section_3Num))
            {
                ++num;
            }
        }
    }  
    
    STF::AreEqual(5, num);
}
