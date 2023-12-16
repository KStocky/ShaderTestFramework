#pragma once

#include "Platform.h"
#include "Utility/Concepts.h"

template<HLSLBaseType T, u32 InNumComponents>
struct HLSLVector;

template<HLSLBaseType T>
struct HLSLVector<T, 2>
{
    constexpr HLSLVector() = default;

    constexpr explicit HLSLVector(const T InBoth) noexcept
        : HLSLVector(InBoth, InBoth)
    {}

    constexpr HLSLVector(const T InX, const T InY) noexcept
        : x(InX)
        , y(InY)
    {}

    template<u32 Index>
        requires (Index < 2)
    constexpr T Get() const noexcept
    {
        if constexpr (Index == 0)
        {
            return x;
        }
        else
        {
            return y;
        }
    }

    constexpr friend HLSLVector operator- (const HLSLVector InA) { return HLSLVector(-InA.x, -InA.y); }
    constexpr friend HLSLVector operator+ (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x + InB.x, InA.y + InB.y); }
    constexpr friend HLSLVector operator- (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x - InB.x, InA.y - InB.y); }
    constexpr friend HLSLVector operator* (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x * InB.x, InA.y * InB.y); }
    constexpr friend HLSLVector operator/ (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x / InB.x, InA.y / InB.y); }
    constexpr friend HLSLVector operator+ (const HLSLVector InA, const T InB) { return InA + HLSLVector(InB); }
    constexpr friend HLSLVector operator- (const HLSLVector InA, const T InB) { return InA - HLSLVector(InB); }
    constexpr friend HLSLVector operator* (const HLSLVector InA, const T InB) { return InA * HLSLVector(InB); }
    constexpr friend HLSLVector operator/ (const HLSLVector InA, const T InB) { return InA / HLSLVector(InB); }
    constexpr friend HLSLVector operator* (const T InA, const HLSLVector InB) { return HLSLVector(InA) * InB; }

    constexpr HLSLVector& operator+=(const T InA) { *this = *this + InA; return *this; }
    constexpr HLSLVector& operator*=(const T InA) { *this = *this * InA; return *this; }
    constexpr HLSLVector& operator-=(const T InA) { *this = *this - InA; return *this; }
    constexpr HLSLVector& operator/=(const T InA) { *this = *this / InA; return *this; }

    constexpr HLSLVector& operator+=(const HLSLVector InA) { *this = *this + InA; return *this; }
    constexpr HLSLVector& operator*=(const HLSLVector InA) { *this = *this * InA; return *this; }
    constexpr HLSLVector& operator-=(const HLSLVector InA) { *this = *this - InA; return *this; }
    constexpr HLSLVector& operator/=(const HLSLVector InA) { *this = *this / InA; return *this; }

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
};

template<HLSLBaseType T>
struct HLSLVector<T, 3>
{
    constexpr HLSLVector() = default;

    constexpr explicit HLSLVector(const T InAll) noexcept
        : HLSLVector(InAll, InAll, InAll)
    {}

    constexpr HLSLVector(const T InX, const T InY, const T InZ) noexcept
        : x(InX)
        , y(InY)
        , z(InZ)
    {}

    template<u32 Index>
        requires (Index < 3)
    constexpr T Get() const
    {
        if constexpr (Index == 0)
        {
            return x;
        }
        else if constexpr (Index == 1)
        {
            return y;
        }
        else
        {
            return z;
        }
    }

    constexpr friend HLSLVector operator- (const HLSLVector InA) { return HLSLVector(-InA.x, -InA.y, -InA.z); }
    constexpr friend HLSLVector operator+ (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x + InB.x, InA.y + InB.y, InA.z + InB.z); }
    constexpr friend HLSLVector operator- (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x - InB.x, InA.y - InB.y, InA.z - InB.z); }
    constexpr friend HLSLVector operator* (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x * InB.x, InA.y * InB.y, InA.z * InB.z); }
    constexpr friend HLSLVector operator/ (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x / InB.x, InA.y / InB.y, InA.z / InB.z); }
    constexpr friend HLSLVector operator+ (const HLSLVector InA, const T InB) { return InA + HLSLVector(InB); }
    constexpr friend HLSLVector operator- (const HLSLVector InA, const T InB) { return InA - HLSLVector(InB); }
    constexpr friend HLSLVector operator* (const HLSLVector InA, const T InB) { return InA * HLSLVector(InB); }
    constexpr friend HLSLVector operator/ (const HLSLVector InA, const T InB) { return InA / HLSLVector(InB); }
    constexpr friend HLSLVector operator* (const T InA, const HLSLVector InB) { return HLSLVector(InA) * InB; }

    constexpr HLSLVector& operator+=(const T InA) { *this = *this + InA; return *this; }
    constexpr HLSLVector& operator*=(const T InA) { *this = *this * InA; return *this; }
    constexpr HLSLVector& operator-=(const T InA) { *this = *this - InA; return *this; }
    constexpr HLSLVector& operator/=(const T InA) { *this = *this / InA; return *this; }

    constexpr HLSLVector& operator+=(const HLSLVector InA) { *this = *this + InA; return *this; }
    constexpr HLSLVector& operator*=(const HLSLVector InA) { *this = *this * InA; return *this; }
    constexpr HLSLVector& operator-=(const HLSLVector InA) { *this = *this - InA; return *this; }
    constexpr HLSLVector& operator/=(const HLSLVector InA) { *this = *this / InA; return *this; }

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
    T z = static_cast<T>(0);
};

template<HLSLBaseType T>
struct HLSLVector<T, 4>
{
    constexpr HLSLVector() = default;

    constexpr explicit HLSLVector(const T InAll) noexcept
        : HLSLVector(InAll, InAll, InAll, InAll)
    {}

    constexpr HLSLVector(const T InX, const T InY, const T InZ, const T InW) noexcept
        : x(InX)
        , y(InY)
        , z(InZ)
        , w(InW)
    {}

    template<u32 Index>
        requires (Index < 4)
    constexpr T Get() const
    {
        if constexpr (Index == 0)
        {
            return x;
        }
        else if constexpr (Index == 1)
        {
            return y;
        }
        else if constexpr (Index == 2)
        {
            return z;
        }
        else
        {
            return w;
        }
    }

    constexpr friend HLSLVector operator- (const HLSLVector InA) { return HLSLVector(-InA.x, -InA.y, -InA.z, -InA.w); }
    constexpr friend HLSLVector operator+ (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x + InB.x, InA.y + InB.y, InA.z + InB.z, InA.w + InB.w); }
    constexpr friend HLSLVector operator- (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x - InB.x, InA.y - InB.y, InA.z - InB.z, InA.w - InB.w); }
    constexpr friend HLSLVector operator* (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x * InB.x, InA.y * InB.y, InA.z * InB.z, InA.w * InB.w); }
    constexpr friend HLSLVector operator/ (const HLSLVector InA, const HLSLVector InB) { return HLSLVector(InA.x / InB.x, InA.y / InB.y, InA.z / InB.z, InA.w / InB.w); }
    constexpr friend HLSLVector operator+ (const HLSLVector InA, const T InB) { return InA + HLSLVector(InB); }
    constexpr friend HLSLVector operator- (const HLSLVector InA, const T InB) { return InA - HLSLVector(InB); }
    constexpr friend HLSLVector operator* (const HLSLVector InA, const T InB) { return InA * HLSLVector(InB); }
    constexpr friend HLSLVector operator/ (const HLSLVector InA, const T InB) { return InA / HLSLVector(InB); }
    constexpr friend HLSLVector operator* (const T InA, const HLSLVector InB) { return HLSLVector(InA) * InB; }

    constexpr HLSLVector& operator+=(const T InA) { *this = *this + InA; return *this; }
    constexpr HLSLVector& operator*=(const T InA) { *this = *this * InA; return *this; }
    constexpr HLSLVector& operator-=(const T InA) { *this = *this - InA; return *this; }
    constexpr HLSLVector& operator/=(const T InA) { *this = *this / InA; return *this; }

    constexpr HLSLVector& operator+=(const HLSLVector InA) { *this = *this + InA; return *this; }
    constexpr HLSLVector& operator*=(const HLSLVector InA) { *this = *this * InA; return *this; }
    constexpr HLSLVector& operator-=(const HLSLVector InA) { *this = *this - InA; return *this; }
    constexpr HLSLVector& operator/=(const HLSLVector InA) { *this = *this / InA; return *this; }

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
    T z = static_cast<T>(0);
    T w = static_cast<T>(0);
};

using float2 = HLSLVector<float, 2>;
using float3 = HLSLVector<float, 3>;
using float4 = HLSLVector<float, 4>;

using int2 = HLSLVector<i32, 2>;
using int3 = HLSLVector<i32, 3>;
using int4 = HLSLVector<i32, 4>;

using uint2 = HLSLVector<u32, 2>;
using uint3 = HLSLVector<u32, 3>;
using uint4 = HLSLVector<u32, 4>;