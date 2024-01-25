#pragma once

#include "Platform.h"
#include "Utility/Concepts.h"

#include <format>

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

    friend constexpr auto operator<=>(const HLSLVector&, const HLSLVector&) = default;

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
};

template<>
struct HLSLVector<bool, 2> : HLSLVector<u32, 2>
{
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

    friend constexpr auto operator<=>(const HLSLVector&, const HLSLVector&) = default;

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
    T z = static_cast<T>(0);
};

template<>
struct HLSLVector<bool, 3> : HLSLVector<u32, 3>
{
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

    friend constexpr auto operator<=>(const HLSLVector&, const HLSLVector&) = default;

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);
    T z = static_cast<T>(0);
    T w = static_cast<T>(0);
};

template<>
struct HLSLVector<bool, 4> : HLSLVector<u32, 4>
{
};

using bool2 = HLSLVector<bool, 2>;
using bool3 = HLSLVector<bool, 3>;
using bool4 = HLSLVector<bool, 4>;

using float16_t2 = HLSLVector<f16, 2>;
using float16_t3 = HLSLVector<f16, 3>;
using float16_t4 = HLSLVector<f16, 4>;

using float2 = HLSLVector<f32, 2>;
using float3 = HLSLVector<f32, 3>;
using float4 = HLSLVector<f32, 4>;

using float64_t2 = HLSLVector<f64, 2>;
using float64_t3 = HLSLVector<f64, 3>;
using float64_t4 = HLSLVector<f64, 4>;

using int16_t2 = HLSLVector<i16, 2>;
using int16_t3 = HLSLVector<i16, 3>;
using int16_t4 = HLSLVector<i16, 4>;

using int2 = HLSLVector<i32, 2>;
using int3 = HLSLVector<i32, 3>;
using int4 = HLSLVector<i32, 4>;

using int64_t2 = HLSLVector<i64, 2>;
using int64_t3 = HLSLVector<i64, 3>;
using int64_t4 = HLSLVector<i64, 4>;

using uint16_t2 = HLSLVector<u16, 2>;
using uint16_t3 = HLSLVector<u16, 3>;
using uint16_t4 = HLSLVector<u16, 4>;

using uint2 = HLSLVector<u32, 2>;
using uint3 = HLSLVector<u32, 3>;
using uint4 = HLSLVector<u32, 4>;

using uint64_t2 = HLSLVector<u64, 2>;
using uint64_t3 = HLSLVector<u64, 3>;
using uint64_t4 = HLSLVector<u64, 4>;

template <typename T>
struct std::formatter<HLSLVector<T, 2>> : std::formatter<string_view> {
    auto format(const HLSLVector<T, 2>& In, auto& ctx) const {
        std::string temp;
        std::format_to(std::back_inserter(temp), "({}, {})",
            static_cast<T>(In.x), static_cast<T>(In.y));
        return std::formatter<string_view>::format(temp, ctx);
    }
};

template <typename T>
struct std::formatter<HLSLVector<T, 3>> : std::formatter<string_view> {
    auto format(const HLSLVector<T, 3>& In, auto& ctx) const {
        std::string temp;
        std::format_to(std::back_inserter(temp), "({}, {}, {})",
            static_cast<T>(In.x), static_cast<T>(In.y), static_cast<T>(In.z));
        return std::formatter<string_view>::format(temp, ctx);
    }
};

template <typename T>
struct std::formatter<HLSLVector<T, 4>> : std::formatter<string_view> {
    auto format(const HLSLVector<T, 4>& In, auto& ctx) const {
        std::string temp;
        std::format_to(std::back_inserter(temp), "({}, {}, {}, {})",
            static_cast<T>(In.x), static_cast<T>(In.y), static_cast<T>(In.z), static_cast<T>(In.w));
        return std::formatter<string_view>::format(temp, ctx);
    }
};

