// Use header guards for shader header files rather than #pragma once
// #pragma once will work. However there is a bug with edit and continue in pix.
// If you try to recompile a shader in pix and #pragma once is used in the shader, it will fail to compile
#ifndef CONDITIONAL_TYPE_HEADER_GUARD
#define CONDITIONAL_TYPE_HEADER_GUARD

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

#endif