#pragma once

#include "Platform.h"

#include "Framework/AssertionsV1/Results.h"
#include "Framework/AssertionsV1/TestDataBufferLayout.h"
#include "Framework/TypeByteReader.h"
#include "Utility/HLSLTypes.h"

#include <span>
#include <string>
#include <vector>

namespace stf::AssertionsV1
{
    std::vector<FailedAssert> ProcessFailedAsserts(const TestDataSection<HLSLAssertMetaData>& InAssertSection, const u32 InNumFailed, const std::span<const std::byte> InTestData, const MultiTypeByteReaderMap& InByteReaderMap);
    std::vector<std::string> ProcessStrings(const TestDataSection<StringMetaData>& InStringSection, const u32 InNumStrings, const std::span<const std::byte> InTestData);
    std::vector<SectionInfoMetaData> ProcessSectionInfo(const TestDataSection<SectionInfoMetaData>& InSectionInfoSection, const u32 InNumSections, const std::span<const std::byte> InTestData);

    TestRunResults ProcessTestDataBuffer(
        const AllocationBufferData InAllocationBufferData,
        const TestDataBufferLayout& InLayout,
        std::span<const std::byte> InTestData,
        const MultiTypeByteReaderMap& InByteReaderMap);
}
