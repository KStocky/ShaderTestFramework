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

template<typename T, uint N>
struct MyArray
{
    T Data[N];
    uint Num;
    static const uint Capacity = N;

    void Init()
    {
        Num = 0;
    }

    bool PushBack(T In)
    {
        if (Num < Capacity)
        {
            Data[Num++] = In;
            return true;
        }

        return false;
    }

    bool PopBack()
    {
        if (Num > 0 && Num < Capacity)
        {
            Num--;
            return true;
        }
        return false;
    }

    Optional<T> operator[](const uint InIndex)
    {
        Optional<T> ret;
        ret.Reset();
        if (InIndex < Num)
        {
            ret.Set(Data[InIndex]);
        }
        return ret;
    }
};