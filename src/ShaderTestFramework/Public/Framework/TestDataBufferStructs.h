#pragma once

#include "Platform.h"

namespace STF
{
    struct DynamicSectionDataInfo
    {
        u32 DataAddress = 0;
        u32 DataSize = 0;
        friend auto operator<=>(const DynamicSectionDataInfo&, const DynamicSectionDataInfo&) = default;
    };

    struct AssertMetaData
    {
        u32 LineNumber = 0;
        u32 ThreadId = 0;
        u32 ThreadIdType = 0;

        friend auto operator<=>(const AssertMetaData&, const AssertMetaData&) = default;
    };

    struct HLSLAssertMetaData : AssertMetaData
    {
        u16 TypeId = 0;
        u16 ReaderId = 0;
        DynamicSectionDataInfo DynamicDataInfo;
        friend auto operator<=>(const HLSLAssertMetaData&, const HLSLAssertMetaData&) = default;
    };

    struct StringMetaData
    {
        DynamicSectionDataInfo DynamicDataInfo;
    };

    struct SectionInfoMetaData
    {
        u32 SectionId = 0;
        u32 StringId = 0;
        u32 ParentId = 0;
        friend auto operator<=>(const SectionInfoMetaData&, const SectionInfoMetaData&) = default;
    };
}