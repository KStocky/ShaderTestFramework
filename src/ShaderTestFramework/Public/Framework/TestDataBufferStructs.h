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
        i32 SectionId = -1;

        friend auto operator<=>(const AssertMetaData&, const AssertMetaData&) = default;
    };

    struct HLSLAssertMetaData : AssertMetaData
    {
        u16 TypeId = 0;
        u16 ReaderId = 0;
        DynamicSectionDataInfo DynamicDataInfo;
        u32 Padding0 = 0;
        friend auto operator<=>(const HLSLAssertMetaData&, const HLSLAssertMetaData&) = default;
    };

    struct StringMetaData
    {
        DynamicSectionDataInfo DynamicDataInfo;
    };

    struct SectionInfoMetaData
    {
        i32 SectionId = -1;
        i32 StringId = -1;
        i32 ParentId = -1;
        friend auto operator<=>(const SectionInfoMetaData&, const SectionInfoMetaData&) = default;
    };
}