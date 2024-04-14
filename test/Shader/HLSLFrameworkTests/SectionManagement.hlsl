#include "/Test/STF/ShaderTestFramework.hlsli"

struct EmptyCallable
{
    void operator()(int, int)
    {
    }
};

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSection_WHEN_Ran_THEN_SectionsEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
    }  
    
    STF::AreEqual(1, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSubsection_WHEN_RanUsingIf_THEN_SectionsEntered2Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
        static const int Section_1Num = 1;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;

            ShaderTestPrivate::Scratch.OnLeave();
        }
    }  
    
    STF::AreEqual(2, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered2Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
        static const int Section_1Num = 1;
        while (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
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
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
        static const int Section_1Num = 1;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
            ShaderTestPrivate::Scratch.OnLeave();
        }
        
        static const int Section_2Num = 2;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num;
            ShaderTestPrivate::Scratch.OnLeave();
        }
    }  
    
    STF::AreEqual(4, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubsections_WHEN_RanUsingWhile_THEN_SectionsEntered4Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
        static const int Section_1Num = 1;
        while (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
        }
        
        static const int Section_2Num = 2;
        while (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_2Num))
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
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;

        static const int Section_1Num = 1;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
            ShaderTestPrivate::Scratch.OnLeave();
        }

        static const int Section_2Num = 2;
        if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num;

            static const int Section_3Num = 3;
            if (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_3Num))
            {
                ++num;
                ShaderTestPrivate::Scratch.OnLeave();
            }
            ShaderTestPrivate::Scratch.OnLeave();
        }
    }  
    
    STF::AreEqual(5, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered5Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    ShaderTestPrivate::Scratch.Init();
    int num = 0;
    while (ShaderTestPrivate::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;

        static const int Section_1Num = 1;
        while (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
        }

        static const int Section_2Num = 2;
        while (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num;

            static const int Section_3Num = 3;
            while (ShaderTestPrivate::Scratch.TryEnterSection(nullCallable, Section_3Num))
            {
                ++num;
            }
        }
    }  
    
    STF::AreEqual(5, num);
}
