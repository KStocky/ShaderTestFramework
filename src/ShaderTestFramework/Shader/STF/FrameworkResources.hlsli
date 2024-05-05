#ifndef STF_FRAME_RESOURCES_HEADER
#define STF_FRAME_RESOURCES_HEADER

#include "/Test/TTL/type_traits.hlsli"

namespace ShaderTestPrivate
{
    template<typename MetaDataType>
    struct TestDataSection
    {
        uint Begin;
        uint NumMeta;
        uint SizeData;
        uint SizeSection;

        uint BeginMeta()
        {
            return Begin;
        }

        uint Num()
        {
            return NumMeta;
        }

        uint SizeInBytesOfMeta()
        {
            return ttl::size_of<MetaDataType>::value * NumMeta;
        }

        uint BeginData()
        {
            return BeginMeta() + SizeInBytesOfMeta();
        }

        uint SizeInBytesOfData()
        {
            return SizeData;
        }

        uint SizeInBytesOfSection()
        {
            return SizeSection;
        }
    };

    struct DynamicSectionDataInfo
    {
        uint DataAddress;
        uint DataSize;
    };

    struct HLSLAssertMetaData
    {
        uint LineNumber;
        uint ThreadId;
        uint ThreadIdType;
        int SectionId;
        uint ReaderAndTypeId;
        DynamicSectionDataInfo DynamicDataInfo;
        uint Padding0;
    };

    struct StringMetaData
    {
        DynamicSectionDataInfo DynamicDataInfo;
    };

    struct SectionInfoMetaData
    {
        int SectionId;
        int StringId;
        int ParentId;
    };

    const uint3 DispatchDimensions;
    const uint AllocationBufferIndex;
    const uint TestDataBufferIndex;

    const TestDataSection<HLSLAssertMetaData> Asserts;
    const TestDataSection<StringMetaData> Strings;
    const TestDataSection<SectionInfoMetaData> Sections;

    RWByteAddressBuffer GetTestDataBuffer()
    {
        return ResourceDescriptorHeap[TestDataBufferIndex];
    }

    static const uint NumSuccessIndex = 0;
    static const uint NumFailsIndex = 4;
    static const uint AssertDataSizeIndex = 8;

    static const uint NumStringsIndex = 12;
    static const uint StringDataSizeIndex = 16;

    static const uint NumSectionsIndex = 20;

    globallycoherent RWByteAddressBuffer GetAllocationBuffer()
    {
        return ResourceDescriptorHeap[AllocationBufferIndex];
    }
}

#endif
