#include "Framework/AssertBufferProcessor.h"

#include <format>

std::vector<std::string> STF::ProcessAssertBuffer(
    const u32 InNumSuccessful, 
    const u32 InNumFailed,
    const AssertBufferLayout,
    std::span<const std::byte>, 
    const TypeConverterMap&)
{
    std::vector<std::string> ret;

    if (InNumFailed == 0)
    {
        return ret;
    }

    ret.push_back(std::format("There were {} successful asserts and {} failed assertions", InNumSuccessful, InNumFailed));

    return ret;
}
