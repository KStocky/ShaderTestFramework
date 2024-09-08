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
    stf::detail::Scratch.Init();
    int num = 0;
    while (stf::detail::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
    }  
    
    ASSERT(AreEqual, 1, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_SingleSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered2Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    stf::detail::Scratch.Init();
    int num = 0;
    while (stf::detail::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
        static const int Section_1Num = 1;
        while (stf::detail::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
        }
    }  
    
    ASSERT(AreEqual, 2, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubsections_WHEN_RanUsingWhile_THEN_SectionsEntered4Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    stf::detail::Scratch.Init();
    int num = 0;
    while (stf::detail::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;
        static const int Section_1Num = stf::detail::Scratch.NextSectionID++;
        while (stf::detail::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
        }
        static const int Section_2Num = stf::detail::Scratch.NextSectionID++;
        while (stf::detail::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num;
        }
    }  
    
    ASSERT(AreEqual, 4, num);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_TwoSubSectionsWithOneNestedSubsection_WHEN_RanUsingWhile_THEN_SectionsEntered5Times(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    EmptyCallable nullCallable;
    stf::detail::Scratch.Init();
    int num = 0;
    while (stf::detail::Scratch.TryLoopScenario(nullCallable))
    {
        ++num;

        static const int Section_1Num = stf::detail::Scratch.NextSectionID++;
        while (stf::detail::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
        }

        static const int Section_2Num = stf::detail::Scratch.NextSectionID++;
        while (stf::detail::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num;

            static const int Section_3Num = stf::detail::Scratch.NextSectionID++;
            while (stf::detail::Scratch.TryEnterSection(nullCallable, Section_3Num))
            {
                ++num;
            }
        }
    }  
    
    ASSERT(AreEqual, 5, num);
}
