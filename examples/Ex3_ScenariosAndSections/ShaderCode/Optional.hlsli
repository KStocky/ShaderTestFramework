#pragma once

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
