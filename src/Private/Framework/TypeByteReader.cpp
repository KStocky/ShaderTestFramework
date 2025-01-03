
#include "Framework/TypeByteReader.h"

#include <ranges>

namespace stf
{
    TypeReaderIndex::TypeReaderIndex(u32 InIndex)
        : m_Index(InIndex)
    {
    }

    TypeReaderIndex::operator u32() const
    {
        return GetIndex();
    }

    u32 TypeReaderIndex::GetIndex() const
    {
        return m_Index;
    }

    std::string DefaultByteReader(const u16, std::span<const std::byte> InBytes)
    {
        std::string ret;

        if (InBytes.empty() || (InBytes.size() % 4 != 0))
        {
            return ret;
        }

        auto toUint = [](const auto InChunk)
            {
                u32 ret = 0;
                std::memcpy(&ret, InChunk.data(), sizeof(u32));
                return ret;
            };

        std::format_to(std::back_inserter(ret), "Bytes: {:#x}", toUint(InBytes));

        for (const auto& byte : InBytes | std::views::drop(4) | std::views::chunk(4))
        {
            std::format_to(std::back_inserter(ret), ", {:#x}", toUint(byte));
        }

        return ret;
    }
}
