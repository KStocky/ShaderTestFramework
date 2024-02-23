#pragma once

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

    struct DynamicUserDataInfo
    {
        uint DataAddress;
        uint DataSize;
    };

    struct HLSLAssertMetaData
    {
        uint LineNumber;
        uint ThreadId;
        uint ThreadIdType;
        uint ReaderAndTypeId;
        DynamicUserDataInfo UserDataInfo;
    };

    struct FormattedStringMetaData
    {
        DynamicUserDataInfo UserDataInfo;
    };

    const uint3 DispatchDimensions;
    const uint AllocationBufferIndex;
    const uint TestDataBufferIndex;

    const TestDataSection<HLSLAssertMetaData> Asserts;
    const TestDataSection<FormattedStringMetaData> Strings;

    RWByteAddressBuffer GetTestDataBuffer()
    {
        return ResourceDescriptorHeap[TestDataBufferIndex];
    }

    globallycoherent RWByteAddressBuffer GetAllocationBuffer()
    {
        return ResourceDescriptorHeap[AllocationBufferIndex];
    }
}