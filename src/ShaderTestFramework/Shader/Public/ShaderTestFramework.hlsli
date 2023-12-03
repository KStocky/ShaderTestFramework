#pragma once

#define SHADER_TEST_RS \
"RootFlags(" \
    "DENY_VERTEX_SHADER_ROOT_ACCESS |" \
    "DENY_HULL_SHADER_ROOT_ACCESS |" \
    "DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
    "DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
    "DENY_PIXEL_SHADER_ROOT_ACCESS |" \
    "DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |" \
    "DENY_MESH_SHADER_ROOT_ACCESS |" \
    "CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED" \
")," \
"RootConstants(" \
    "num32BitConstants=7," \
    "b0" \
")"

namespace ShaderTestPrivate
{
    const uint3 DispatchDimensions;
    const uint MaxNumAsserts;
    const uint AssertBufferIndex;
    const uint ScratchBufferIndex;
    const uint ScratchBufferSize;
    static const uint AssertFailureNumBytes = 16;
    
    RWByteAddressBuffer GetAssertBuffer()
    {
        return ResourceDescriptorHeap[AssertBufferIndex];
    }
    
    void Success()
    {
        uint successIndex;
        GetAssertBuffer().InterlockedAdd(0, 1, successIndex);
    }
    
    void AddError()
    {
        uint assertIndex;
        GetAssertBuffer().InterlockedAdd(4, 1, assertIndex);
    }
}

namespace ShaderTestPrivate
{
    static int NextSectionID = 0;
    static const int NumSections = 32;
}

namespace ShaderTestPrivate
{
    static int CurrentSectionID = 0;
    
    struct ScenarioSectionInfo
    {
        int ParentID;
        bool HasBeenEntered;
        bool HasSubsectionBeenEntered;
        bool HasUnenteredSubsections;
    };
    
    static ScenarioSectionInfo Sections[NumSections];
    
    bool TryEnterSection(int InID)
    {
        const bool shouldEnter = !Sections[InID].HasBeenEntered || Sections[InID].HasUnenteredSubsections;
        
        if (shouldEnter)
        {
            if (InID == 0)
            {
                CurrentSectionID = 0;
                Sections[InID].HasBeenEntered = true;
                Sections[InID].HasSubsectionBeenEntered = false;
                
                return true;
            }
            else
            {
                const bool ourTurn = !Sections[CurrentSectionID].HasSubsectionBeenEntered;
                if (ourTurn)
                {
                    Sections[CurrentSectionID].HasSubsectionBeenEntered = true;
                    Sections[CurrentSectionID].HasUnenteredSubsections = false;
                    Sections[InID].ParentID = CurrentSectionID;
                    Sections[InID].HasBeenEntered = true;
                    CurrentSectionID = InID;
                    return true;
                }
                else
                {
                    Sections[CurrentSectionID].HasUnenteredSubsections = true;
                }
            }
        }
        
        return false;
    }
    
    void OnLeave()
    {
        CurrentSectionID = Sections[CurrentSectionID].ParentID;
    }
    
    struct PerThreadScratchData
    {
        int CurrentSectionID;
        ScenarioSectionInfo Sections[NumSections];
    };
    
    RWStructuredBuffer<PerThreadScratchData> GetScratchBuffer()
    {
        return ResourceDescriptorHeap[ScratchBufferIndex];
    }
    
    bool TryEnterSection(uint InThreadId, int InID)
    {
        const bool shouldEnter = !GetScratchBuffer()[InThreadId].Sections[InID].HasBeenEntered || GetScratchBuffer()[InThreadId].Sections[InID].HasUnenteredSubsections;
        
        if (shouldEnter)
        {
            if (InID == 0)
            {
                GetScratchBuffer()[InThreadId].CurrentSectionID = 0;
                GetScratchBuffer()[InThreadId].Sections[InID].HasBeenEntered = true;
                GetScratchBuffer()[InThreadId].Sections[InID].HasSubsectionBeenEntered = false;
                
                return true;
            }
            else
            {
                const bool ourTurn = !GetScratchBuffer()[InThreadId].Sections[GetScratchBuffer()[InThreadId].CurrentSectionID].HasSubsectionBeenEntered;
                if (ourTurn)
                {
                    GetScratchBuffer()[InThreadId].Sections[GetScratchBuffer()[InThreadId].CurrentSectionID].HasSubsectionBeenEntered = true;
                    GetScratchBuffer()[InThreadId].Sections[GetScratchBuffer()[InThreadId].CurrentSectionID].HasUnenteredSubsections = false;
                    GetScratchBuffer()[InThreadId].Sections[InID].ParentID = GetScratchBuffer()[InThreadId].CurrentSectionID;
                    GetScratchBuffer()[InThreadId].Sections[InID].HasBeenEntered = true;
                    GetScratchBuffer()[InThreadId].CurrentSectionID = InID;
                    return true;
                }
                else
                {
                    GetScratchBuffer()[InThreadId].Sections[GetScratchBuffer()[InThreadId].CurrentSectionID].HasUnenteredSubsections = true;
                }
            }
        }
        
        return false;
    }
    
    void OnLeave(uint InThreadId)
    {
        GetScratchBuffer()[InThreadId].CurrentSectionID = GetScratchBuffer()[InThreadId].Sections[GetScratchBuffer()[InThreadId].CurrentSectionID].ParentID;
    }
}

namespace STF
{
    template<typename T, T v>
    struct integral_constant
    {
        static const T value = v;
        using value_type = T;
        using type = integral_constant;
    };
    
    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;
    
    template<typename T, typename U>
    struct is_same : false_type
    {
    };
    
    template<typename T>
    struct is_same<T, T> : true_type 
    {
    };
    
    template<bool InCond, typename T = void>
    struct enable_if
    {
    };
    
    template<typename T>
    struct enable_if<true, T>
    {
        using type = T;  
    };
    
    template<typename To, typename From>
    To Cast(From In);
    
    template<>
    bool Cast<bool, bool>(bool In)
    {
        return In;
    }

    uint FlattenIndex(const uint3 InIndex, const uint3 InDimensions)
    {
        return (InIndex.z * InDimensions.x * InDimensions.y) + (InIndex.y * InDimensions.x) + InIndex.x;
    }
    
    template<typename T, typename U>
    typename enable_if<is_same<T, U>::value>::type AreEqual(const T InA, const U InB)
    {
        if (all(InA == InB))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError();
        }
    }
    
    template<typename T, typename U>
    typename enable_if<is_same<T, U>::value>::type NotEqual(const T InA, const U InB)
    {
        if (any(InA != InB))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError();
        }
    }
    
    template<typename T>
    void IsTrue(T In)
    {
        if (Cast<bool>(In))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError();
        }
    }
    
    template<typename T>
    void IsFalse(T In)
    {
        if (!Cast<bool>(In))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError();
        }
    }
}

#define STF_GET_SECTION_VAR_NAME(InLine) STF_Section_##InLine##_Var
#define STF_GET_SECTION_VAR_VALUE(InLine) STF_Section_##InLine##_Var_Value
#define STF_CREATE_SECTION_VAR_IMPL(InLine) \
    static const int STF_GET_SECTION_VAR_NAME(InLine) = ShaderTestPrivate::NextSectionID++; \

#define STF_CREATE_SECTION_VAR STF_CREATE_SECTION_VAR_IMPL(__LINE__)

#define SCENARIO()
#define BEGIN_SECTION()
#define END_SECTION()

//[RootSignature(SHADER_TEST_RS)]
//[numthreads(1, 1, 1)]
//void MyTestScenario(uint3 DispatchThreadId : SV_DispatchThreadID)
//{
//    SCENARIO()
//    {
//        STF::AreEqual(5, 5);
//        
//        BEGIN_SECTION()
//            STF::IsTrue(true);
//        END_SECTION()
//        
//        BEGIN_SECTION()
//            STF::IsFalse(false);
//            
//            BEGIN_SECTION()
//                STF::NotEqual(5, 4);
//            END_SECTION()
//        END_SECTION()
//    }
//}
//
//#define BEGIN_SCENARIO(InName, ...)
//#define END_SCENARIO()
//
//[RootSignature(SHADER_TEST_RS)]
//[numthreads(1, 1, 1)]
//BEGIN_SCENARIO(MyTestScenario, uint3 DispatchThreadId : SV_DispatchThreadID)
//
//    STF::AreEqual(5, 5);
//        
//    BEGIN_SECTION() 
//        STF::IsTrue(true);
//    END_SECTION() 
//        
//    BEGIN_SECTION() 
//        STF::IsFalse(false);
//            
//        BEGIN_SECTION() 
//            STF::NotEqual(5, 4);
//        END_SECTION() 
//    END_SECTION() 
//END_SCENARIO()