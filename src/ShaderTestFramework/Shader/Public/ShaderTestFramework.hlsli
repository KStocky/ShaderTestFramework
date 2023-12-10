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
    "num32BitConstants=1," \
    "b0" \
")"

namespace ShaderTestPrivate
{
    const uint AssertBufferIndex;
    
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
        Scratch.NextSectionID = 1;
        for (uint i = 0; i < NumSections; ++i)
        {
            Scratch.Sections[i].ParentID = i == 0 ? -1 : 0;
            Scratch.Sections[i].HasBeenEntered = false;
            Scratch.Sections[i].HasSubsectionBeenEntered = false;
            Scratch.Sections[i].HasUnenteredSubsections = false;
        }
    }
    
    bool ShouldEnter(int InID)
    {
        return !Scratch.Sections[InID].HasBeenEntered || Scratch.Sections[InID].HasUnenteredSubsections;
    }
    
    bool TryLoopScenario()
    {
        if (ShouldEnter(0))
        {
            Scratch.CurrentSectionID = 0;
            Scratch.Sections[0].HasBeenEntered = true;
            Scratch.Sections[0].HasSubsectionBeenEntered = false;
                
            return true;
        }
        
        return false;
    }
    
    bool TryEnterSection(int InID)
    {
        if (!ShouldEnter(InID))
        {
            return false;
        }

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
            
            return false;
        }
    }
    
    void OnLeave()
    {
        Scratch.Sections[Scratch.CurrentSectionID].HasSubsectionBeenEntered = false;
        const bool hasUnenteredSubsections = Scratch.Sections[Scratch.CurrentSectionID].HasUnenteredSubsections;
        Scratch.CurrentSectionID = Scratch.Sections[Scratch.CurrentSectionID].ParentID;
        Scratch.Sections[Scratch.CurrentSectionID].HasUnenteredSubsections = Scratch.Sections[Scratch.CurrentSectionID].HasUnenteredSubsections || hasUnenteredSubsections;
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

    template<typename T>
    struct array_traits
    {
        static const bool is_array = false;
        static const uint size = 0;
        using element_type = void;
    };

    template<typename T, uint Size>
    struct array_traits<T[Size]>
    {
        static const bool is_array = true;
        static const uint size = Size;
        using element_type = T;
    };

    template<typename T>
    struct container_traits
    {
        static const bool is_container = false;
        static const bool is_writable = false;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = false;
        using element_type = void;
    };

    template<typename T, uint Size>
    struct container_traits<T[Size]>
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = false;
        using element_type = T;
    };

    template<typename T>
    struct container_traits<Buffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = false;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = T;
    };

    template<typename T>
    struct container_traits<RWBuffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = false;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = T;
    };

    template<typename T>
    struct container_traits<StructuredBuffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = false;
        static const bool is_byte_address = false;
        static const bool is_structured = true;
        static const bool is_resource = true;
        using element_type = T;
    };

    template<typename T>
    struct container_traits<RWStructuredBuffer<T> >
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = false;
        static const bool is_structured = true;
        static const bool is_resource = true;
        using element_type = T;
    };

    template<>
    struct container_traits<ByteAddressBuffer>
    {
        static const bool is_container = true;
        static const bool is_writable = false;
        static const bool is_byte_address = true;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = uint;
    };

    template<>
    struct container_traits<RWByteAddressBuffer>
    {
        static const bool is_container = true;
        static const bool is_writable = true;
        static const bool is_byte_address = true;
        static const bool is_structured = false;
        static const bool is_resource = true;
        using element_type = uint;
    };

    template<typename ContainerType, typename = void>
    struct container;
    
    template<typename T>
    struct container<T, typename enable_if<container_traits<T>::is_container>::type>
    {
        using underlying_type = T;
        using element_type = typename container_traits<underlying_type>::element_type;
        static const bool writable = container_traits<underlying_type>::is_writable;

        underlying_type Data;

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && array_traits<U>::is_array, uint>::type size()
        {
            return array_traits<U>::size;
        }

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && !array_traits<U>::is_array, uint>::type size()
        {
            uint ret = 0;
            Data.GetDimensions(ret);
            return ret;
        }

        uint size_in_bytes()
        {
            return sizeof(element_type) * size();
        }

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && !array_traits<U>::is_array, uint>::type load(const uint InIndex)
        {
            return Data.Load(InIndex);
        }

        template<typename U = T>
        typename enable_if<is_same<U, T>::value && array_traits<U>::is_array, uint>::type load(const uint InIndex)
        {
            return Data[InIndex];
        }

        template<typename U>
        struct BufferEnabler
        {
            static const bool value = writable && is_same<element_type, U>::value && !container_traits<underlying_type>::is_byte_address;
        };

        template<typename U>
        struct ByteAddressEnabler
        {
            static const bool value = writable && is_same<element_type, U>::value && container_traits<underlying_type>::is_byte_address;
        };

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem)
        {
            Data[InIndex] = InItem;
        }

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2)
        {
            Data[InIndex] = InItem;
            Data[InIndex + 1] = InItem2;
        }

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3)
        {
            Data[InIndex] = InItem;
            Data[InIndex + 1] = InItem2;
            Data[InIndex + 2] = InItem3;
        }

        template<typename U>
        typename enable_if<BufferEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3, const U InItem4)
        {
            Data[InIndex] = InItem;
            Data[InIndex + 1] = InItem2;
            Data[InIndex + 2] = InItem3;
            Data[InIndex + 3] = InItem4;
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem)
        {
            Data.Store(InIndex, InItem);
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2)
        {
            Data.Store(InIndex, uint2(InItem, InItem2));
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3)
        {
            Data.Store(InIndex, uint3(InItem, InItem2, InItem3));
        }

        template<typename U>
        typename enable_if<ByteAddressEnabler<U>::value>::type store(const uint InIndex, const U InItem, const U InItem2, const U InItem3, const U InItem4)
        {
            Data.Store(InIndex, uint4(InItem, InItem2, InItem3, InItem4));
        }
    };

    template<typename T>
    struct container_traits<container<T> >
    {
        static const bool is_container = container_traits<T>::is_container;
        static const bool is_writable = container_traits<T>::is_writable;
        static const bool is_byte_address = container_traits<T>::is_byte_address;
        static const bool is_structured = container_traits<T>::is_structured;
        static const bool is_resource = container_traits<T>::is_resource;
        using element_type = typename container_traits<T>::element_type;
    };
}

#ifndef TYPE_ID_BOOL
#define TYPE_ID_BOOL 1
#endif

namespace STF
{
    template<typename T>
    struct type_id : integral_constant<uint, 0>
    {};

    template<> struct type_id<bool> : integral_constant<uint, TYPE_ID_BOOL>{};

    template<> struct type_id<int> : integral_constant<uint, TYPE_ID_INT>{};
    template<> struct type_id<int2> : integral_constant<uint, TYPE_ID_INT2>{};
    template<> struct type_id<int3> : integral_constant<uint, TYPE_ID_INT3>{};
    template<> struct type_id<int4> : integral_constant<uint, TYPE_ID_INT4>{};

    template<> struct type_id<uint> : integral_constant<uint, TYPE_ID_UINT>{};
    template<> struct type_id<uint2> : integral_constant<uint, TYPE_ID_UINT2>{};
    template<> struct type_id<uint3> : integral_constant<uint, TYPE_ID_UINT3>{};
    template<> struct type_id<uint4> : integral_constant<uint, TYPE_ID_UINT4>{};

    template<> struct type_id<float> : integral_constant<uint, TYPE_ID_FLOAT>{};
    template<> struct type_id<float2> : integral_constant<uint, TYPE_ID_FLOAT2>{};
    template<> struct type_id<float3> : integral_constant<uint, TYPE_ID_FLOAT3>{};
    template<> struct type_id<float4> : integral_constant<uint, TYPE_ID_FLOAT4>{};
}

namespace STF
{
    template<typename T>
    struct is_fundamental : false_type{};

    template<> struct is_fundamental<bool> : true_type{};
    template<> struct is_fundamental<int> : true_type{};
    template<> struct is_fundamental<int2> : true_type{};
    template<> struct is_fundamental<int3> : true_type{};
    template<> struct is_fundamental<int4> : true_type{};
    template<> struct is_fundamental<uint> : true_type{};
    template<> struct is_fundamental<uint2> : true_type{};
    template<> struct is_fundamental<uint3> : true_type{};
    template<> struct is_fundamental<uint4> : true_type{};
    template<> struct is_fundamental<float> : true_type{};
    template<> struct is_fundamental<float2> : true_type{};
    template<> struct is_fundamental<float3> : true_type{};
    template<> struct is_fundamental<float4> : true_type{};
}

namespace STF
{
    template<typename T>
    struct fundamental_type_traits;

    template<typename T, uint InRank>
    struct fundamental_type_traits_base
    {
        using base_type = T;
        static const uint rank = InRank;
    };

    template<> struct fundamental_type_traits<bool> : fundamental_type_traits_base<bool, 1>{};

    template<> struct fundamental_type_traits<uint> : fundamental_type_traits_base<uint, 1>{};
    template<> struct fundamental_type_traits<uint2> : fundamental_type_traits_base<uint, 2>{};
    template<> struct fundamental_type_traits<uint3> : fundamental_type_traits_base<uint, 3>{};
    template<> struct fundamental_type_traits<uint4> : fundamental_type_traits_base<uint, 4>{};

    template<> struct fundamental_type_traits<int> : fundamental_type_traits_base<int, 1>{};
    template<> struct fundamental_type_traits<int2> : fundamental_type_traits_base<int, 2>{};
    template<> struct fundamental_type_traits<int3> : fundamental_type_traits_base<int, 3>{};
    template<> struct fundamental_type_traits<int4> : fundamental_type_traits_base<int, 4>{};

    template<> struct fundamental_type_traits<float> : fundamental_type_traits_base<float, 1>{};
    template<> struct fundamental_type_traits<float2> : fundamental_type_traits_base<float, 2>{};
    template<> struct fundamental_type_traits<float3> : fundamental_type_traits_base<float, 3>{};
    template<> struct fundamental_type_traits<float4> : fundamental_type_traits_base<float, 4>{};
}

namespace STF
{
    template<typename T, typename = void>
    struct ByteWriter
    {
        static const bool HasWriter = false;
    };

    template<typename T>
    struct ByteWriter<T, typename enable_if<is_fundamental<T>::value>::type>
    {
        static const bool HasWriter = true;
        static const bool IsBoolWriter = is_same<typename fundamental_type_traits<T>::base_type, bool>::value;

        static uint BytesRequired(T)
        {
            return sizeof(T);
        }

        template<typename ContainerType, uint InRank, bool ForBools = false>
        struct WriteEnabler
        {
            static const bool cond_for_bools = 
                is_same<typename ContainerType::element_type, uint>::value && 
                ForBools;
            
            static const bool cond_for_non_bools = 
                is_same<typename ContainerType::element_type, uint>::value && 
                fundamental_type_traits<T>::rank == InRank;
            static const bool value = IsBoolWriter ? cond_for_bools : cond_for_non_bools;
        };

        template<typename U>
        static typename enable_if<WriteEnabler<container<U>, 1>::value>::type Write(inout container<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In));
        }

        template<typename U>
        static typename enable_if<WriteEnabler<container<U>, 2>::value>::type Write(inout container<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In.x), asuint(In.y));
        }

        template<typename U>
        static typename enable_if<WriteEnabler<container<U>, 3>::value>::type Write(inout container<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In.x), asuint(In.y), asuint(In.z));
        }

        template<typename U>
        static typename enable_if<WriteEnabler<container<U>, 4>::value>::type Write(inout container<U> InContainer, const uint InIndex, const T In)
        {
            InContainer.store(InIndex, asuint(In.x), asuint(In.y), asuint(In.z), asuint(In.w));
        }
        
        template<typename U>
        static typename enable_if<WriteEnabler<container<U>, 0, true>::value>::type Write(inout container<U> InContainer, const uint InIndex, const T In)
        {
            ByteWriter<uint>::Write(InContainer, InIndex, In ? 1u : 0u);
        }
    };
}

namespace STF
{
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
    static const int STF_GET_SECTION_VAR_NAME(InID) = ShaderTestPrivate::Scratch.NextSectionID++ \

#define STF_CREATE_SECTION_VAR STF_CREATE_SECTION_VAR_IMPL(__LINE__)

#define STF_GET_TEST_FUNC_NAME(InID) STF_TEST_FUNC_##InID
#define STF_DECLARE_TEST_FUNC(InID) void STF_GET_TEST_FUNC_NAME(InID)()

#define STF_DEFINE_TEST_ENTRY_FUNC(InID, InName, ...) void InName(__VA_ARGS__) \
{\
    ShaderTestPrivate::InitScratch();\
    STF_DECLARE_TEST_FUNC(InID); \
    while(ShaderTestPrivate::TryLoopScenario())\
    {\
        STF_GET_TEST_FUNC_NAME(InID)();\
    }\
}\
STF_DECLARE_TEST_FUNC(InID)

#define STF_SCENARIO_IMPL(InID)\
ShaderTestPrivate::InitScratch();\
while(ShaderTestPrivate::TryLoopScenario())


#define SCENARIO() STF_SCENARIO_IMPL(__LINE__)

#define STF_BEGIN_SECTION_IMPL(InID) STF_CREATE_SECTION_VAR_IMPL(InID); \
    if (ShaderTestPrivate::TryEnterSection(STF_GET_SECTION_VAR_NAME(InID))) \
    {\

#define BEGIN_SECTION STF_BEGIN_SECTION_IMPL(__LINE__)

#define END_SECTION ShaderTestPrivate::OnLeave(); }
