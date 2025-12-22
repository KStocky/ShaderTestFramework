#pragma once

#include "Utility/Exception.h"
#include "Utility/Concepts.h"
#include "Platform.h"

#include <format>

namespace stf
{

    template<typename BackingType, u32 NumIndexBits>
        requires 
            ((sizeof(BackingType) * 8) > NumIndexBits) &&
            CValidBitField<BackingType, NumIndexBits, (sizeof(BackingType) * 8) - NumIndexBits>
    class VersionedIndex
    {
    public:

        static constexpr u32 NumVersionBits = (sizeof(BackingType) * 8) - NumIndexBits;
        static constexpr BackingType MaxIndex = (1u << NumIndexBits) - 1u;
        static constexpr BackingType MaxVersion = (1u << NumVersionBits) - 1u;

        VersionedIndex() = default;
        VersionedIndex(const BackingType InIndex)
            : VersionedIndex(InIndex, 0)
        {
        }

        VersionedIndex Next() const
        {
            return VersionedIndex{ m_Index, m_Version + 1 };
        }

        BackingType GetIndex() const
        {
            return m_Index;
        }

        BackingType GetVersion() const
        {
            return m_Version;
        }

        friend bool operator==(const VersionedIndex&, const VersionedIndex&) = default;
        friend bool operator!=(const VersionedIndex&, const VersionedIndex&) = default;

    private:

        VersionedIndex(const BackingType InIndex, const BackingType InVersion)
            : m_Index(InIndex)
            , m_Version(InVersion & MaxVersion)
        {
            ThrowIfFalse(InIndex <= MaxIndex, std::format("Provided index ({}) is not in range [0, {}]", InIndex, MaxIndex));
        }

        BackingType m_Index : NumIndexBits{};
        BackingType m_Version : NumVersionBits{};
    };
    
    using u32VersionedIndex = VersionedIndex<u32, 24>;
}