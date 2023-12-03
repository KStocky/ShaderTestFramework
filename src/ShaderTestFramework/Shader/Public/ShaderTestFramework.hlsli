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
    "num32BitConstants=2," \
    "b0" \
")"

namespace ShaderTestPrivate
{
    const uint MaxNumAsserts;
    const uint AssertBufferIndex;
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
    struct ScenarioSectionInfo
    {
        int ParentID;
        bool HasBeenEntered;
        bool HasSubsectionBeenEntered;
        bool HasUnenteredSubsections;
    };
    
    struct PerThreadScratchData
    {
        int CurrentSectionID;
        int NextSectionID;
        ScenarioSectionInfo Sections[NumSections];
    };
    
    static PerThreadScratchData Scratch;
    
    void InitScratch()
    {
        Scratch.CurrentSectionID = 0;
        for (uint i = 0; i < NumSections; ++i)
        {
            Scratch.Sections[i].ParentID = 0;
            Scratch.Sections[i].HasBeenEntered = false;
            Scratch.Sections[i].HasSubsectionBeenEntered = false;
            Scratch.Sections[i].HasUnenteredSubsections = false;
        }
    }
    
    bool TryEnterSection(int InID)
    {
        const bool shouldEnter = !Scratch.Sections[InID].HasBeenEntered || Scratch.Sections[InID].HasUnenteredSubsections;
        
        if (shouldEnter)
        {
            if (InID == 0)
            {
                Scratch.CurrentSectionID = 0;
                Scratch.Sections[InID].HasBeenEntered = true;
                Scratch.Sections[InID].HasSubsectionBeenEntered = false;
                
                return true;
            }
            else
            {
                const bool ourTurn = !Scratch.Sections[Scratch.CurrentSectionID].HasSubsectionBeenEntered;
                if (ourTurn)
                {
                    Scratch.Sections[Scratch.CurrentSectionID].HasSubsectionBeenEntered = true;
                    Scratch.Sections[Scratch.CurrentSectionID].HasUnenteredSubsections = false;
                    Scratch.Sections[InID].ParentID = Scratch.CurrentSectionID;
                    Scratch.Sections[InID].HasBeenEntered = true;
                    Scratch.CurrentSectionID = InID;
                    return true;
                }
                else
                {
                    Scratch.Sections[Scratch.CurrentSectionID].HasUnenteredSubsections = true;
                }
            }
        }
        
        return false;
    }
    
    void OnLeave()
    {
        Scratch.CurrentSectionID = Scratch.Sections[Scratch.CurrentSectionID].ParentID;
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

    void Fail()
    {
        ShaderTestPrivate::AddError();
    }
}

#define STF_GET_SECTION_VAR_NAME(InID) STF_Section_##InID##_Var
#define STF_CREATE_SECTION_VAR_IMPL(InID) \
    static const int STF_GET_SECTION_VAR_NAME(InID) = ShaderTestPrivate::NextSectionID++ \

#define STF_CREATE_SECTION_VAR STF_CREATE_SECTION_VAR_IMPL(__LINE__)

#define STF_GET_TEST_FUNC_NAME(InID) STF_TEST_FUNC_##InID
#define STF_DECLARE_TEST_FUNC(InID) void STF_GET_TEST_FUNC_NAME(InID)()

#define STF_DEFINE_TEST_ENTRY_FUNC(InID, InName, ...) void InName(__VA_ARGS__) \
{\
    ShaderTestPrivate::InitScratch();\
    STF_DECLARE_TEST_FUNC(InID); \
    STF_CREATE_SECTION_VAR_IMPL(InID);\
    while(ShaderTestPrivate::TryEnterSection(STF_GET_SECTION_VAR_NAME(InID)))\
    {\
        STF_GET_TEST_FUNC_NAME(InID)();\
    }\
}\
STF_DECLARE_TEST_FUNC(InID)

#define SCENARIO(InName, ...) STF_DEFINE_TEST_ENTRY_FUNC(__LINE__, InName, __VA_ARGS__)

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
//SCENARIO(MyTestScenario, uint3 DispatchThreadId : SV_DispatchThreadID)
//{
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
//}