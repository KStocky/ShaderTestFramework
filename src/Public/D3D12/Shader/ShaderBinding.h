#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

namespace stf
{

    template<typename T>
    concept CShaderBindingDataType = std::is_trivially_copyable_v<T>;

    class ShaderBinding
    {
    public:

        template<CShaderBindingDataType T>
        ShaderBinding(std::string InName, const T& InData)
            : m_Name(std::move(InName))
            , m_Data(sizeof(T))
        {
            std::memcpy(m_Data.data(), &InData, sizeof(T));
        }

        std::string_view GetName() const;
        std::span<const std::byte> GetBindingData() const;

    private:
        std::string m_Name;
        std::vector<std::byte> m_Data;
    };
}