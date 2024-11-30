#include "/Test/STF/ShaderTestFramework.hlsli"

struct EmptyCallable
{
    void operator()(int, int)
    {
    }
};

[numthreads(1,1,1)]
void GIVEN_EmptyScenario_WHEN_Ran_THEN_NoAssertMade(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
    }
}

[numthreads(1,1,1)]
void GIVEN_ScenarioWithNoSections_WHEN_Ran_THEN_FunctionOnlyEnteredOnce(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
        static int counter = 0;
        const int num = counter++;
        ASSERT(AreEqual, num, 0);
    }
}

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
        if (stf::detail::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num;
            stf::detail::Scratch.OnLeave();
        }

        ASSERT(AreEqual, 1, num);
        ASSERT(AreEqual, 1, numEntered);
    }
}

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
        if (stf::detail::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num1;
            stf::detail::Scratch.OnLeave();
        }
        
        static const int Section_2Num = 2;
        if (stf::detail::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num2;
            stf::detail::Scratch.OnLeave();
        }

        if (numEntered == 1)
        {
            ASSERT(AreEqual, 1, num1);
            ASSERT(AreEqual, 0, num2);
        }
        else if (numEntered == 2)
        {
            ASSERT(AreEqual, 1, num1);
            ASSERT(AreEqual, 1, num2);
        }
        else
        {
            ASSERT(Fail);
        }
    }
}

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
        if (stf::detail::Scratch.TryEnterSection(nullCallable, Section_1Num))
        {
            ++num1;
            stf::detail::Scratch.OnLeave();
        }

        static const int Section_2Num = 2;
        if (stf::detail::Scratch.TryEnterSection(nullCallable, Section_2Num))
        {
            ++num2;

            static const int Section_3Num = 3;
            if (stf::detail::Scratch.TryEnterSection(nullCallable, Section_3Num))
            {
                ++num3;
                stf::detail::Scratch.OnLeave();
            }
            stf::detail::Scratch.OnLeave();
        }
    
        if (numEntered == 1)
        {
            ASSERT(AreEqual, 1, num1);
            ASSERT(AreEqual, 0, num2);
            ASSERT(AreEqual, 0, num3);
        }
        else if (numEntered == 2)
        {
            ASSERT(AreEqual, 1, num1);
            ASSERT(AreEqual, 1, num2);
            ASSERT(AreEqual, 1, num3);
        }
        else
        {
            ASSERT(Fail);
        }
    }
}

[numthreads(1,1,1)]
void GIVEN_ScenarioWithoutId_WHEN_Ran_THEN_IdIsNone(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    SCENARIO("")
    {
    }
    ASSERT(AreEqual, 0u, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::None, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(42,1,1)]
void GIVEN_ScenarioWithDispatchThreadId_WHEN_Ran_THEN_IdIsInt3(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    stf::RegisterThreadID(DispatchThreadId);
    SCENARIO("")
    {
    }
    ASSERT(AreEqual, stf::detail::FlattenIndex(DispatchThreadId, stf::detail::DispatchDimensions), stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int3, stf::detail::Scratch.ThreadID.Type);
}

[numthreads(42,1,1)]
void GIVEN_ScenarioWithIntId_WHEN_Ran_THEN_IdIsInt()
{
    stf::RegisterThreadID(42);
    SCENARIO("")
    {
    }
    ASSERT(AreEqual, 42u, stf::detail::Scratch.ThreadID.Data);
    ASSERT(AreEqual, stf::detail::EThreadIDType::Int, stf::detail::Scratch.ThreadID.Type);
}