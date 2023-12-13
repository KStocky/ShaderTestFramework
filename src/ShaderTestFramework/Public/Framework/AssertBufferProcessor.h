#pragma once

#include "Platform.h"

#include <functional>
#include <unordered_map>
#include <span>
#include <string>
#include <vector>

namespace STF
{
    struct HLSLAssertMetaData
    {
        u32 LineNumber = 0;
        u32 ThreadId = 0;
        u32 ThreadIdType = 0;
        u32 TypeId = 0;
        u32 DataAddress = 0;
        u32 DataSize = 0;
    };

    using TypeConverterMap = std::unordered_map<u32, std::function<std::string(std::span<const std::byte>)>>;

    std::vector<std::string> ProcessAssertBuffer(
        const u32 InNumSuccessful,
        const u32 InNumFailed,
        std::span<const std::byte> InAssertData,
        const TypeConverterMap& InTypeHandlerMap);
}