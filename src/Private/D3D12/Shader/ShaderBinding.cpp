
#include "D3D12/Shader/ShaderBinding.h"

namespace stf
{
    std::string_view ShaderBinding::GetName() const
    {
        return m_Name;
    }

    std::span<const std::byte> ShaderBinding::GetBindingData() const
    {
        return m_Data;
    }
}