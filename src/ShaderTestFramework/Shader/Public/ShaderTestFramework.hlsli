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
    "num32BitConstants=8," \
    "b0" \
")"

namespace ShaderTestPrivate
{
    static const int NumSections = 32;
}

namespace ShaderTestPrivate
{
    enum class ESectionRunState
    {
        NeedsRun,
        Running,
        RunningEnteredSubsection,
        RunningNeedsRerun,
        Completed
    };

    struct ScenarioSectionInfo
    {
        int ParentID;
        ESectionRunState RunState; 
    };

    enum class EThreadIDType
    {
        None,
        Int,
        Int3
    };

    struct ThreadIDInfo
    {
        uint Data;
        EThreadIDType Type;
    };
    
    struct PerThreadScratchData
    {
        int CurrentSectionID;
        int NextSectionID;
        ThreadIDInfo ThreadID;
        ScenarioSectionInfo Sections[NumSections];
    };
    
    static PerThreadScratchData Scratch;
    
    void InitScratch()
    {
        Scratch.CurrentSectionID = 0;
        Scratch.NextSectionID = 1;
        Scratch.ThreadID.Data = 0;
        Scratch.ThreadID.Type = EThreadIDType::None;
        for (uint i = 0; i < NumSections; ++i)
        {
            Scratch.Sections[i].ParentID = i == 0 ? -1 : 0;
            Scratch.Sections[i].RunState = ESectionRunState::NeedsRun;
        }
    }

    void OnLeave(int InID = Scratch.CurrentSectionID)
    {
        const bool hasUnenteredSubsections = Scratch.Sections[InID].RunState == ESectionRunState::RunningNeedsRerun;
        Scratch.Sections[InID].RunState = hasUnenteredSubsections ? ESectionRunState::NeedsRun : ESectionRunState::Completed;
        Scratch.CurrentSectionID = Scratch.Sections[InID].ParentID;
        Scratch.Sections[Scratch.CurrentSectionID].RunState = hasUnenteredSubsections ?
            ESectionRunState::RunningNeedsRerun :
            Scratch.Sections[Scratch.CurrentSectionID].RunState;
    
    }

     bool ShouldEnter(int InID)
    {   
        const ESectionRunState state = Scratch.Sections[InID].RunState;
        switch (state)
        {
            case ESectionRunState::NeedsRun:
            {
                return true;
            }
            case ESectionRunState::RunningNeedsRerun:
            case ESectionRunState::RunningEnteredSubsection:
            case ESectionRunState::Running:
            {
                OnLeave(InID);
                return false;
            }
            case ESectionRunState::Completed:
            {
                return false;
            }
        }

        return false;
    }

    bool TryLoopScenario()
    {
        const bool shouldEnter = 
            Scratch.Sections[0].RunState == ESectionRunState::NeedsRun || 
            Scratch.Sections[0].RunState == ESectionRunState::RunningNeedsRerun;

        if (shouldEnter)
        {
            Scratch.CurrentSectionID = 0;
            Scratch.Sections[0].RunState = ESectionRunState::Running;
                
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

        const bool ourTurn = Scratch.Sections[Scratch.CurrentSectionID].RunState == ESectionRunState::Running;
        if (ourTurn)
        {
            Scratch.Sections[Scratch.CurrentSectionID].RunState = ESectionRunState::RunningEnteredSubsection;
            Scratch.Sections[InID].ParentID = Scratch.CurrentSectionID;
            Scratch.Sections[InID].RunState = ESectionRunState::Running;
            Scratch.CurrentSectionID = InID;
            return true;
        }
        else
        {   
            Scratch.Sections[Scratch.CurrentSectionID].RunState = ESectionRunState::RunningNeedsRerun;  
            return false;
        }
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

namespace ShaderTestPrivate
{
    const uint AssertBufferIndex;
    const uint3 DispatchDimensions;
    const uint MaxNumAsserts;
    const uint SizeInBytesOfAssertData;
    const uint SizeInBytesOfAssertBuffer;
    const uint AllocationBufferIndex;

    struct HLSLAssertMetaData
    {
        uint LineNumber;
        uint ThreadId;
        uint ThreadIdType;
        uint TypeId;
        uint DataAddress;
        uint DataSize;
    };
    
    RWByteAddressBuffer GetAssertBuffer()
    {
        return ResourceDescriptorHeap[AssertBufferIndex];
    }

    globallycoherent RWByteAddressBuffer GetAllocationBuffer()
    {
        return ResourceDescriptorHeap[AllocationBufferIndex];
    }
    
    void Success()
    {
        uint successIndex;
        GetAllocationBuffer().InterlockedAdd(0, 1, successIndex);
    }

    uint StartAddressAssertData()
    {
        return sizeof(HLSLAssertMetaData) * MaxNumAsserts;
    }
    
    uint AddAssert()
    {
        uint assertIndex;
        GetAllocationBuffer().InterlockedAdd(4, 1, assertIndex);
        return assertIndex;
    }

    void AddAssertMetaInfo(const uint InMetaIndex, const uint InId, const uint InTypeId, const uint2 InAddressAndSize)
    {
        RWByteAddressBuffer buffer = GetAssertBuffer();
        const uint metaAddress = InMetaIndex * sizeof(HLSLAssertMetaData);
        buffer.Store4(metaAddress, uint4(InId, Scratch.ThreadID.Data, (uint)Scratch.ThreadID.Type, InTypeId));
        buffer.Store2(metaAddress + 16, InAddressAndSize);
    }

    template<typename T>
    typename STF::enable_if<STF::ByteWriter<T>::HasWriter, uint2>::type AddAssertData(T In1, T In2)
    {
        using Writer = STF::ByteWriter<T>;
        const uint size1 = Writer::BytesRequired(In1);
        const uint size2 = Writer::BytesRequired(In2);
        const uint size = size1 + size2 + 8;
        uint offset = 0;
        GetAllocationBuffer().InterlockedAdd(8, size, offset);

        const uint address = StartAddressAssertData() + offset;
        if (address + size < SizeInBytesOfAssertBuffer)
        {
            STF::container<RWByteAddressBuffer> buffer;
            buffer.Data = GetAssertBuffer();
            buffer.store(address, size1);
            Writer::Write(buffer, address + 4, In1);
            buffer.store(address + 4 + size1, size2);
            Writer::Write(buffer, address + 8 + size2, In2);
            return uint2(address, size);
        }

        return uint2(0, 0);
    }

    template<typename T>
    typename STF::enable_if<!STF::ByteWriter<T>::HasWriter, uint2>::type AddAssertData(T In1, T In2)
    {
        return uint2(0, 0);
    }

    template<typename T>
    typename STF::enable_if<STF::ByteWriter<T>::HasWriter, uint2>::type AddAssertData(T In)
    {
        using Writer = STF::ByteWriter<T>;
        const uint size1 = Writer::BytesRequired(In);
        const uint size = size1 + 4;
        uint offset = 0;
        GetAllocationBuffer().InterlockedAdd(8, size, offset);

        const uint address = StartAddressAssertData() + offset;
        if (address + size < SizeInBytesOfAssertBuffer)
        {
            STF::container<RWByteAddressBuffer> buffer;
            buffer.Data = GetAssertBuffer();
            buffer.store(address, size1);
            Writer::Write(buffer, address + 4, In);
            return uint2(address, size);
        }

        return uint2(0, 0);
    }

    template<typename T>
    typename STF::enable_if<!STF::ByteWriter<T>::HasWriter, uint2>::type AddAssertData(T In)
    {
        return uint2(0, 0);
    }
    
    template<typename T>
    void AddError(T In1, T In2, int InId)
    {
        const uint metaIndex = AddAssert();
        if (metaIndex < MaxNumAsserts)
        {
            uint2 addressAndSize = uint2(0, 0);
            if (SizeInBytesOfAssertData > 0)
            {
                addressAndSize = AddAssertData(In1, In2);
            }
            AddAssertMetaInfo(metaIndex, InId, STF::type_id<T>::value, addressAndSize);
        }
    }

    template<typename T>
    void AddError(T In, int InId)
    {
        const uint metaIndex = AddAssert();
        if (metaIndex < MaxNumAsserts)
        {
            uint2 addressAndSize = uint2(0, 0);
            if (SizeInBytesOfAssertData > 0)
            {
                addressAndSize = AddAssertData(In);
            }
            
            AddAssertMetaInfo(metaIndex, InId, STF::type_id<T>::value, addressAndSize);
        }
    }
}

namespace STF
{
    void RegisterThreadID(uint InID)
    {
        ShaderTestPrivate::Scratch.ThreadID.Type = ShaderTestPrivate::EThreadIDType::Int;
        ShaderTestPrivate::Scratch.ThreadID.Data = InID;
    }

    void RegisterThreadID(uint3 InID)
    {
        ShaderTestPrivate::Scratch.ThreadID.Type = ShaderTestPrivate::EThreadIDType::Int3;
        ShaderTestPrivate::Scratch.ThreadID.Data = STF::FlattenIndex(InID, ShaderTestPrivate::DispatchDimensions);
    }
}

namespace STF
{
    template<typename T, typename U>
    typename enable_if<is_same<T, U>::value>::type AreEqual(const T InA, const U InB, int InId = -1)
    {
        if (all(InA == InB))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError(InA, InB, InId);
        }
    }
    
    template<typename T, typename U>
    typename enable_if<is_same<T, U>::value>::type NotEqual(const T InA, const U InB, int InId = -1)
    {
        if (any(InA != InB))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError(InA, InB, InId);
        }
    }
    
    template<typename T>
    void IsTrue(T In, int InId = -1)
    {
        if (Cast<bool>(In))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError(In, InId);
        }
    }
    
    template<typename T>
    void IsFalse(T In, int InId = -1)
    {
        if (!Cast<bool>(In))
        {
            ShaderTestPrivate::Success();
        }
        else
        {
            ShaderTestPrivate::AddError(In, InId);
        }
    }

    void Fail(int InId = -1)
    {
        ShaderTestPrivate::AddError(false, InId);
    }
}

#define STF_JOIN(a, b) a##b

#define STF_NUM_ARGS_N( \
          _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9, _10, \
         _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
         _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
         _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, \
         _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, \
         _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, \
         _61, _62, _63, N, ...) N

#define STF_NUM_ARGS_RSEQ_N()                                        \
         62, 61, 60,                                       \
         59, 58, 57, 56, 55, 54, 53, 52, 51, 50,           \
         49, 48, 47, 46, 45, 44, 43, 42, 41, 40,           \
         39, 38, 37, 36, 35, 34, 33, 32, 31, 30,           \
         29, 28, 27, 26, 25, 24, 23, 22, 21, 20,           \
         19, 18, 17, 16, 15, 14, 13, 12, 11, 10,           \
          9,  8,  7,  6,  5,  4,  3,  2,  1,  0

#define STF_NUM_ARGS_IMPL(...)    STF_NUM_ARGS_N(__VA_ARGS__)    

#define STF_NUM_ARGS(...)     STF_NUM_ARGS_IMPL(Ignored, ##__VA_ARGS__, STF_NUM_ARGS_RSEQ_N())

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

#define STF_SCENARIO_IF_0(InScenarioID)\
ShaderTestPrivate::InitScratch();\
while(ShaderTestPrivate::TryLoopScenario())

#define STF_SCENARIO_IF_1(InScenarioID, InThreadID)\
ShaderTestPrivate::InitScratch();\
STF::RegisterThreadID(InThreadID); \
while(ShaderTestPrivate::TryLoopScenario())

#define STF_SCENARIO_IMPL(InName, InNumArgs, InScenarioID, ...) STF_JOIN(InName, InNumArgs)(InScenarioID, ##__VA_ARGS__)

#define SCENARIO(...) STF_SCENARIO_IMPL(STF_SCENARIO_IF_, STF_NUM_ARGS(__VA_ARGS__), __LINE__, ##__VA_ARGS__)

#define STF_BEGIN_SECTION_IMPL(InID) STF_CREATE_SECTION_VAR_IMPL(InID); \
    if (ShaderTestPrivate::TryEnterSection(STF_GET_SECTION_VAR_NAME(InID))) \
    {\

#define BEGIN_SECTION STF_BEGIN_SECTION_IMPL(__LINE__)
#define END_SECTION ShaderTestPrivate::OnLeave(); }

#define STF_SECTION_IMPL(InID) STF_CREATE_SECTION_VAR_IMPL(InID); \
    while (ShaderTestPrivate::TryEnterSection(STF_GET_SECTION_VAR_NAME(InID)))

#define SECTION() STF_SECTION_IMPL(__LINE__)

#define STF_ASSERT_IF_0(InName, InId) STF::InName(InId)
#define STF_ASSERT_IF_1(InName, InId, InArg) STF::InName(InArg, InId)
#define STF_ASSERT_IF_2(InName, InId, InArg1, InArg2) STF::InName(InArg1, InArg2, InId)
#define STF_ASSERT_IMPL(InStem, InNumArgs, InName, InId, ...) STF_JOIN(InStem, InNumArgs)(InName, InId, ##__VA_ARGS__)
#define ASSERT(InName, ...) STF_ASSERT_IMPL(STF_ASSERT_IF_, STF_NUM_ARGS(__VA_ARGS__), InName, __LINE__, ##__VA_ARGS__)
