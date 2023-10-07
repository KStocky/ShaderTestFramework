#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <concepts>
#include <iterator>
#include <ranges>
#include <span>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>


template<class... Ts> 
struct OverloadSet : Ts... 
{ 
    using Ts::operator()...; 
};

template<typename... InTypes>
constexpr bool AlwaysFalse = false;

template<typename FromType, typename TransformFunc>
    requires requires(FromType From, TransformFunc Func)
    {
        {Func(From)};
    }
auto RangeToVector(const std::span<FromType> InRange, TransformFunc&& InFunc)
{
    using ElemType = decltype(InFunc(std::declval<FromType>()));
    std::vector<ElemType> ret;
    ret.reserve(InRange.size());
    std::ranges::copy(std::views::transform(InRange, InFunc), std::back_inserter(ret));
    return ret;
}

template<typename T>
constexpr std::string_view TypeToString()
{
    constexpr std::string_view funcName = __FUNCSIG__;

    const auto startIndex = funcName.find_last_of('<');
    const auto endIndex = funcName.find_last_of('>');
    const auto ret = funcName.substr(startIndex + 1, (endIndex - startIndex) -1);
    return ret;
}

template<typename... T>
void CheckTemplateArgs() 
{
    static_assert(AlwaysFalse<T...>);
}

inline std::wstring ToWString(const std::string& InString)
{
    return std::wstring(InString.cbegin(), InString.cend());
}
