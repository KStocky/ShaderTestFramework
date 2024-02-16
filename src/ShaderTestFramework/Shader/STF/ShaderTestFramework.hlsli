#pragma once

#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/STF/RootSignature.hlsli"
#include "/Test/STF/SectionManagement.hlsli"

#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/caster.hlsli"
#include "/Test/TTL/container_wrapper.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/type_traits.hlsli"

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
        uint ReaderAndTypeId;
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

    void AddAssertMetaInfo(const uint InMetaIndex, const uint InId, const uint InReaderAndTypeId, const uint2 InAddressAndSize)
    {
        RWByteAddressBuffer buffer = GetAssertBuffer();
        const uint metaAddress = InMetaIndex * sizeof(HLSLAssertMetaData);
        buffer.Store4(metaAddress, uint4(InId, Scratch.ThreadID.Data, (uint)Scratch.ThreadID.Type, InReaderAndTypeId));
        buffer.Store2(metaAddress + 16, InAddressAndSize);
    }

    template<typename T>
    uint2 AddAssertData(T In1, T In2)
    {
        const uint size1 = ttl::bytes_required(In1);
        const uint size2 = ttl::bytes_required(In2);
        const uint align1 = ttl::alignment_required(In1);
        const uint align2 = ttl::alignment_required(In2);
        const uint sizeAndAlign1 = (size1 << 16) | align1;
        const uint sizeAndAlign2 = (size2 << 16) | align2;

        const uint alignedSize1 = ttl::aligned_offset(size1 + 4, align1);
        const uint alignedSize2 = ttl::aligned_offset(size2 + 4, align2);
        const uint size = ttl::aligned_offset(alignedSize1 + alignedSize2, 8);
        uint offset = 0;
        GetAllocationBuffer().InterlockedAdd(8, size, offset);

        const uint startAddress = StartAddressAssertData() + offset;
        uint address = startAddress;
        if (address + size < SizeInBytesOfAssertBuffer)
        {
            RWByteAddressBuffer buff = GetAssertBuffer();
            ttl::write_bytes(buff, address, sizeAndAlign1);
            address = ttl::aligned_offset(address + 4, align1);
            ttl::write_bytes(buff, address, In1);
            address = ttl::aligned_offset(address + size1, 4);
            ttl::write_bytes(buff, address, sizeAndAlign2);
            address = ttl::aligned_offset(address + 4, align2);
            ttl::write_bytes(buff, address, In2);

            return uint2(startAddress, size);
        }

        return uint2(0, 0);
    }

    template<typename T>
    uint2 AddAssertData(T In)
    {
        const uint size1 = ttl::bytes_required(In);
        const uint align1 = ttl::alignment_required(In);
        const uint sizeAndAlign1 = (size1 << 16) | align1;

        const uint alignedSize1 = ttl::aligned_offset(size1 + 4, align1);
        const uint size = ttl::aligned_offset(alignedSize1, 8);
        uint offset = 0;
        GetAllocationBuffer().InterlockedAdd(8, size, offset);

        const uint startAddress = StartAddressAssertData() + offset;
        uint address = startAddress;
        if (address + size < SizeInBytesOfAssertBuffer)
        {
            RWByteAddressBuffer buff = GetAssertBuffer();
            ttl::write_bytes(buff, address, sizeAndAlign1);
            address = ttl::aligned_offset(address + 4, align1);
            ttl::write_bytes(buff, address, In);

            return uint2(startAddress, size);
        }

        return uint2(0, 0);
    }

    template<typename T, typename = void>
    struct ByteReaderTraitsEval
    {
        static const uint16_t ReaderId = STF::ByteReaderTraits<T>::ReaderId;
        static const uint16_t TypeId = 0;
    };

    template<typename T>
    struct ByteReaderTraitsEval<T, typename ttl::enable_if<STF::ByteReaderTraits<T>::TypeId != 0>::type> : STF::ByteReaderTraits<T>{};
    
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

            using Traits = ByteReaderTraitsEval<T>;
            const uint32_t readerId = Traits::ReaderId;
            const uint32_t typeId = Traits::TypeId;
            const uint packed = typeId | (readerId << 16);
            AddAssertMetaInfo(metaIndex, InId, packed, addressAndSize);
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

            using Traits = ByteReaderTraitsEval<T>;
            const uint32_t readerId = Traits::ReaderId;
            const uint32_t typeId = Traits::TypeId;
            const uint packed = typeId | (readerId << 16);
            AddAssertMetaInfo(metaIndex, InId, packed, addressAndSize);
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
        ShaderTestPrivate::Scratch.ThreadID.Data = ShaderTestPrivate::FlattenIndex(InID, ShaderTestPrivate::DispatchDimensions);
    }
}

namespace STF
{
    template<typename T, typename U>
    typename ttl::enable_if<ttl::is_same<T, U>::value>::type AreEqual(const T InA, const U InB, int InId = -1)
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
    typename ttl::enable_if<ttl::is_same<T, U>::value>::type NotEqual(const T InA, const U InB, int InId = -1)
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
        if (ttl::cast<bool>(In))
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
        if (!ttl::cast<bool>(In))
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
