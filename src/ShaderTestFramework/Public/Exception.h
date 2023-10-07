#pragma once

#include "Platform.h"
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

std::string HrToString(HRESULT hr)
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

void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

void ThrowIfFalse(const bool InCondition, std::string InMessage = "Condition was not true")
{
    if (!InCondition)
    {
        throw std::runtime_error(std::move(InMessage));
    }
}