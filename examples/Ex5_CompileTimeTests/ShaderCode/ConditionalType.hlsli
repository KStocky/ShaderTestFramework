#pragma once

template<bool InCond, typename IfTrue, typename IfFalse>
struct ConditionalType
{
    using Type = IfTrue;
};

template<typename IfTrue, typename IfFalse>
struct ConditionalType<false, IfTrue, IfFalse>
{
    using Type = IfFalse;
};