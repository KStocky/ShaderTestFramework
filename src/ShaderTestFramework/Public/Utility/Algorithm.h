#pragma once

#include <concepts>
#include <ranges>
#include <span>
#include <type_traits>
#include <vector>

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