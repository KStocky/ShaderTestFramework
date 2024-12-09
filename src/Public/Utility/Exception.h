#pragma once

#include "Platform.h"

#include "Utility/Expected.h"
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace stf
{

    inline std::string HrToString(HRESULT hr)
    {
        return std::format("HRESULT of {:#08x}", static_cast<u32>(hr));
    }

    class HrException : public std::runtime_error
    {
    public:
        HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) { std::cout << HrToString(hr); }
        HRESULT Error() const { return m_hr; }
    private:
        const HRESULT m_hr;
    };

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw HrException(hr);
        }
    }

    inline void ThrowIfFalse(const bool InCondition, std::string InMessage = "Condition was not true")
    {
        if (!InCondition)
        {
            throw std::runtime_error(std::move(InMessage));
        }
    }

    template<typename T, typename ErrorType>
    decltype(auto) ThrowIfUnexpected(Expected<T, ErrorType>&& InExpected, std::string_view InMessage = "Had unexpected value")
    {
        if (!InExpected.has_value())
        {
            throw std::runtime_error(std::string(InMessage));
        }

        if constexpr (std::is_same_v<T, void>)
        {
            return;
        }
        else
        {
            return InExpected.value();
        }
    }

    template<typename T>
    decltype(auto) ThrowIfUnexpected(Expected<T, HRESULT>&& InExpected)
    {
        if (!InExpected.has_value())
        {
            throw HrException(InExpected.error());
        }
        if constexpr (std::is_same_v<T, void>)
        {
            return;
        }
        else
        {
            return InExpected.value();
        }
    }
}