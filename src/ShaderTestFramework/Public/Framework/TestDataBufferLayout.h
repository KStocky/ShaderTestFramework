#pragma once
#include "Platform.h"

#include "Framework/TestDataBufferStructs.h"
#include "Utility/Math.h"

namespace STF
{
    template<typename MetaDataType>
    class TestDataSection
    {
    public:

        constexpr TestDataSection() = default;

        constexpr TestDataSection(u32 InBegin, u32 InNumMeta, u32 InSizeInBytesOfData)
            : m_Begin(static_cast<u32>(AlignedOffset(InBegin, 8ull)))
            , m_NumMeta(InNumMeta)
            , m_SizeData(static_cast<u32>(AlignedOffset(InSizeInBytesOfData, 8ull)))
            , m_SizeSection(m_SizeData + static_cast<u32>(AlignedOffset(InNumMeta * sizeof(MetaDataType), 8ull)))
        {
        }

        constexpr u32 Begin() const
        {
            return m_Begin;
        }

        constexpr u32 NumMeta() const
        {
            return m_NumMeta;
        }

        constexpr u32 SizeInBytesOfData() const
        {
            return m_SizeData;
        }

        constexpr u32 SizeInBytesOfSection() const
        {
            return m_SizeSection;
        }

        constexpr u32 SizeInBytesOfMeta() const
        {
            return m_SizeSection - m_SizeData;
        }

        constexpr u32 BeginData() const
        {
            return m_Begin + SizeInBytesOfMeta();
        }

        constexpr u32 EndSection() const
        {
            return m_Begin + m_SizeSection;
        }

    private:

        constexpr friend auto operator<=>(const TestDataSection&, const TestDataSection&) = default;

        u32 m_Begin = 0;
        u32 m_NumMeta = 0;
        u32 m_SizeData = 0;
        u32 m_SizeSection = 0;
    };

    struct TestDataBufferLayoutDesc
    {
        u32 NumFailedAsserts = 0;
        u32 NumBytesAssertData = 0;
        u32 NumStrings = 0;
        u32 NumBytesStringData = 0;
        u32 NumSections = 0;
    };

    class TestDataBufferLayout
    {
    public:
        TestDataBufferLayout(TestDataBufferLayoutDesc InDesc);
        TestDataBufferLayout(u32 InNumFailedAsserts = 0, u32 InBytesAssertData = 0, u32 InNumStrings = 0, u32 InBytesStringData = 0, u32 InNumSections = 0);

        TestDataSection<HLSLAssertMetaData> GetAssertSection() const;
        TestDataSection<StringMetaData> GetStringSection() const;
        TestDataSection<SectionInfoMetaData> GetSectionInfoSection() const;

        u32 GetSizeOfTestData() const;

    private:
        TestDataSection<HLSLAssertMetaData> m_Asserts;
        TestDataSection<StringMetaData> m_Strings;
        TestDataSection<SectionInfoMetaData> m_Sections;

        friend auto operator<=>(const TestDataBufferLayout&, const TestDataBufferLayout&) = default;
    };
}