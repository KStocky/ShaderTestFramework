#ifndef STF_SHADER_TEST_FRAMEWORK_HEADER
#define STF_SHADER_TEST_FRAMEWORK_HEADER

#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/STF/FrameworkResources.hlsli"
#include "/Test/STF/RootSignature.hlsli"
#include "/Test/STF/SectionManagement.hlsli"

#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/caster.hlsli"
#include "/Test/TTL/container_wrapper.hlsli"
#include "/Test/TTL/macro.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/string.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace ShaderTestPrivate
{
    void Success()
    {
        uint successIndex;
        GetAllocationBuffer().InterlockedAdd(NumSuccessIndex, 1, successIndex);
    }
    
    uint AddAssert()
    {
        uint assertIndex;
        GetAllocationBuffer().InterlockedAdd(NumFailsIndex, 1, assertIndex);
        return assertIndex;
    }

    void AddAssertMetaInfo(const uint InMetaIndex, const uint InId, const uint InReaderAndTypeId, const uint2 InAddressAndSize)
    {
        RWByteAddressBuffer buffer = GetTestDataBuffer();
        const uint metaAddress = InMetaIndex * sizeof(HLSLAssertMetaData);
        buffer.Store4(metaAddress, uint4(InId, Scratch.ThreadID.Data, (uint)Scratch.ThreadID.Type, Scratch.GetSectionID()));
        buffer.Store3(metaAddress + 16, uint3(InReaderAndTypeId, InAddressAndSize));
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
        GetAllocationBuffer().InterlockedAdd(AssertDataSizeIndex, size, offset);

        const uint startAddress = Asserts.BeginData() + offset;
        uint address = startAddress;
        if (offset + size < Asserts.SizeInBytesOfData())
        {
            RWByteAddressBuffer buff = GetTestDataBuffer();
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
        GetAllocationBuffer().InterlockedAdd(AssertDataSizeIndex, size, offset);

        const uint startAddress = Asserts.BeginData() + offset;
        uint address = startAddress;
        if (offset + size < Asserts.SizeInBytesOfData())
        {
            RWByteAddressBuffer buff = GetTestDataBuffer();
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
        if (metaIndex < Asserts.Num())
        {
            uint2 addressAndSize = uint2(0, 0);
            if (Asserts.SizeInBytesOfData() > 0)
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
        if (metaIndex < Asserts.Num())
        {
            uint2 addressAndSize = uint2(0, 0);
            if (Asserts.SizeInBytesOfData() > 0)
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

    template<uint N>
    uint2 AddStringData(ttl::string<N> In)
    {
        const uint size = ttl::aligned_offset(In.Size, 4u);
        uint offset = 0;
        GetAllocationBuffer().InterlockedAdd(StringDataSizeIndex, size, offset);

        const uint startAddress = Strings.BeginData() + offset;
        uint address = startAddress;
        if (offset + size < Strings.SizeInBytesOfData())
        {
            RWByteAddressBuffer buff = GetTestDataBuffer();
            ttl::write_bytes(buff, address, In);

            return uint2(startAddress, size);
        }

        return uint2(0, 0);
    }

    void AddStringMetaInfo(const uint InMetaIndex, const uint2 InAddressAndSize)
    {
        RWByteAddressBuffer buffer = GetTestDataBuffer();
        const uint metaAddress = InMetaIndex * sizeof(StringMetaData) + Strings.BeginMeta();
        buffer.Store2(metaAddress, InAddressAndSize);
    }

    bool ShouldWriteGlobalData(const uint InAllocationAddress, const int InId)
    {
        uint oldIndex;
        GetAllocationBuffer().InterlockedCompareExchange(InAllocationAddress, InId, InId + 1, oldIndex);

        return oldIndex == InId;
    }

    template<uint N>
    void AddGlobalString(const uint InId, ttl::string<N> In)
    {
        const uint stringIndex = InId;
        if (!ShouldWriteGlobalData(NumStringsIndex, InId))
        {
            return;
        }

        if (stringIndex < Strings.Num())
        {
            uint2 addressAndSize = uint2(0, 0);
            if (Strings.SizeInBytesOfData() > 0)
            {
                addressAndSize = AddStringData(In);
            }

            AddStringMetaInfo(stringIndex, addressAndSize);
        }
    }

    void AddGlobalSection(SectionInfoMetaData InSectionInfo)
    {
        const uint sectionIndex = InSectionInfo.SectionId;
        if (!ShouldWriteGlobalData(NumSectionsIndex, InSectionInfo.SectionId))
        {
            return;
        }

        if (sectionIndex < Sections.Num())
        {
            const uint bufferAddress = Sections.BeginMeta() + sectionIndex * ttl::size_of<SectionInfoMetaData>::value;
            GetTestDataBuffer().Store(bufferAddress, InSectionInfo);
        }
    }

    struct OnFirstEntryOfSectionFunctor
    {
        int StringId;
        void operator()(const int InSectionId, const int InParentId)
        {
            SectionInfoMetaData metaData;
            metaData.SectionId = InSectionId;
            metaData.ParentId = InParentId;
            metaData.StringId = StringId;
            AddGlobalSection(metaData);
        }
    };
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

#define STF_GET_SECTION_VAR_NAME(InID) STF_Section_##InID##_Var
#define STF_CREATE_SECTION_VAR_IMPL(InID) \
    static const int STF_GET_SECTION_VAR_NAME(InID) = ShaderTestPrivate::Scratch.NextSectionID++ \

#define STF_CREATE_SECTION_VAR STF_CREATE_SECTION_VAR_IMPL(__LINE__)

#define STF_GET_TEST_FUNC_NAME(InID) STF_TEST_FUNC_##InID
#define STF_DECLARE_TEST_FUNC(InID) void STF_GET_TEST_FUNC_NAME(InID)()

#define STF_DEFINE_TEST_ENTRY_FUNC(InID, InName, ...) void InName(__VA_ARGS__) \
{\
    ShaderTestPrivate::Scratch.Init();\
    STF_DECLARE_TEST_FUNC(InID); \
    while(ShaderTestPrivate::Scratch.TryLoopScenario())\
    {\
        STF_GET_TEST_FUNC_NAME(InID)();\
    }\
}\
STF_DECLARE_TEST_FUNC(InID)

#define STF_SCENARIO_IMPL(InName, InScenarioId)                                                                                   \
CREATE_STRING(TTL_JOIN(scenarioNameString, InScenarioId), InName);                                                         \
static uint TTL_JOIN(scenarioNameId, InScenarioId) = ShaderTestPrivate::Scratch.NextStringID++; \
ShaderTestPrivate::AddGlobalString(TTL_JOIN(scenarioNameId, InScenarioId), TTL_JOIN(scenarioNameString, InScenarioId));    \
ShaderTestPrivate::OnFirstEntryOfSectionFunctor TTL_JOIN(onFirstEntry, InScenarioId);                                                                       \
TTL_JOIN(onFirstEntry, InScenarioId).StringId = TTL_JOIN(scenarioNameId, InScenarioId);                                                                     \
ShaderTestPrivate::Scratch.Init();                                                                                                  \
while(ShaderTestPrivate::Scratch.TryLoopScenario(TTL_JOIN(onFirstEntry, InScenarioId)))

#define SCENARIO(InName) STF_SCENARIO_IMPL(InName, __LINE__)

#define STF_SECTION_IMPL(InName, InID) STF_CREATE_SECTION_VAR_IMPL(InID);                                       \
CREATE_STRING(TTL_JOIN(sectionNameString, InID), InName);                                              \
static uint TTL_JOIN(sectionNameId, InID) = ShaderTestPrivate::Scratch.NextStringID++; \
ShaderTestPrivate::AddGlobalString(TTL_JOIN(sectionNameId, InID), TTL_JOIN(sectionNameString, InID)); \
ShaderTestPrivate::OnFirstEntryOfSectionFunctor TTL_JOIN(onFirstEntry, InID);                                   \
TTL_JOIN(onFirstEntry, InID).StringId = TTL_JOIN(sectionNameId, InID);                                          \
while (ShaderTestPrivate::Scratch.TryEnterSection(TTL_JOIN(onFirstEntry, InID), STF_GET_SECTION_VAR_NAME(InID)))

#define SECTION(InName) STF_SECTION_IMPL(InName, __LINE__)

#define STF_ASSERT_IF_0(InName, InId) STF::InName(InId)
#define STF_ASSERT_IF_1(InName, InId, InArg) STF::InName(InArg, InId)
#define STF_ASSERT_IF_2(InName, InId, InArg1, InArg2) STF::InName(InArg1, InArg2, InId)
#define ASSERT(InName, ...) TTL_JOIN_MACRO(STF_ASSERT_IF_, TTL_NUM_ARGS(__VA_ARGS__), InName, __LINE__, ##__VA_ARGS__)

#endif
