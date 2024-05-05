// Use header guards for shader header files rather than #pragma once
// #pragma once will work. However there is a bug with edit and continue in pix.
// If you try to recompile a shader in pix and #pragma once is used in the shader, it will fail to compile
#ifndef OPTIONAL_EXAMPLE_HEADER_GUARD
#define OPTIONAL_EXAMPLE_HEADER_GUARD

template<typename T>
struct Optional
{
    T Data;
    bool IsValid;

    void Reset()
    {
        Data = (T)0;
        IsValid = false;
    }

    void Set(T In)
    {
        Data = In;
        IsValid = true;
    }

    T GetOrDefault(T InDefault)
    {
        if (IsValid)
        {
            return Data;
        }

        return InDefault;
    }
};

#endif
