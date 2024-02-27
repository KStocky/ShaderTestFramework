#include "Framework/TestDataBufferLayout.h"
#include "Utility/Math.h"

namespace STF
{
    TestDataBufferLayout::TestDataBufferLayout(u32 InNumFailedAsserts, u32 InBytesAssertData, u32 InNumStrings, u32 InBytesStringData)
        : m_Asserts(0, InNumFailedAsserts, static_cast<u32>(AlignedOffset(InBytesAssertData, 8ull)))
        , m_Strings(m_Asserts.EndSection(), InNumStrings, static_cast<u32>(AlignedOffset(InBytesStringData, 8ull)))
    {
    }

    TestDataSection<HLSLAssertMetaData> TestDataBufferLayout::GetAssertSection() const
    {
        return m_Asserts;
    }

    TestDataSection<StringMetaData> TestDataBufferLayout::GetStringSection() const
    {
        return m_Strings;
    }

    const u32 TestDataBufferLayout::GetSizeOfTestData() const
    {
        return m_Asserts.SizeInBytesOfSection() + m_Strings.SizeInBytesOfSection();
    }
}
