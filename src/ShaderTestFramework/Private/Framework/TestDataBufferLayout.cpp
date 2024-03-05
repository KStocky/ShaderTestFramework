#include "Framework/TestDataBufferLayout.h"

namespace STF
{
    TestDataBufferLayout::TestDataBufferLayout(u32 InNumFailedAsserts, u32 InBytesAssertData, u32 InNumStrings, u32 InBytesStringData)
        : m_Asserts(0, InNumFailedAsserts, InBytesAssertData)
        , m_Strings(m_Asserts.EndSection(), InNumStrings, InBytesStringData)
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

    u32 TestDataBufferLayout::GetSizeOfTestData() const
    {
        return m_Asserts.SizeInBytesOfSection() + m_Strings.SizeInBytesOfSection();
    }
}
