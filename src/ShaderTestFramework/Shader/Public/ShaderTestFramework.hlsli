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
    static const int NumTrackers = 32;
    static bool TrackerAllocations[NumTrackers];
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
        if ((bool)In)
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
        if (!(bool)In)
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
    static const int STF_GET_SECTION_VAR_VALUE(InLine) = ShaderTestPrivate::NextSectionID++; \
    static const int STF_GET_SECTION_VAR_NAME(InLine) = STF_GET_SECTION_VAR_VALUE(InLine); \
    ShaderTestPrivate::TrackerAllocations[STF_GET_SECTION_VAR_VALUE(InLine)] = true

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