#pragma once

#include "Platform.h"

#include <span>

#include <unknwn.h>
#include <dxcapi.h>

namespace stf
{
    class ShaderHash
    {
    public:

        ShaderHash() = default;
        ShaderHash(const DxcShaderHash& In) noexcept
            : m_Hash(In)
        {}

        std::span<const u8> GetHash() const
        {
            return m_Hash.HashDigest;
        }

        friend bool operator==(const ShaderHash& InA, const ShaderHash& InB)
        {
            return std::memcmp(&InA.m_Hash, &InB.m_Hash, sizeof(InA.m_Hash)) == 0;
        }

        friend bool operator!=(const ShaderHash& InA, const ShaderHash& InB)
        {
            return !(InA == InB);
        }

    private:

        DxcShaderHash m_Hash;
    };
}

