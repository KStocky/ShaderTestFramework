
// Use header guards for shader header files rather than #pragma once
// #pragma once will work. However there is a bug with edit and continue in pix.
// If you try to recompile a shader in pix and #pragma once is used in the shader, it will fail to compile
#ifndef COOL_HLSL_FUNCTION_HEADER_GUARD
#define COOL_HLSL_FUNCTION_HEADER_GUARD

int ReturnTheBestNumber()
{
    return 42;
}

#endif