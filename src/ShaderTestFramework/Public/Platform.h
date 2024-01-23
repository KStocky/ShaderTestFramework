#pragma once
#include "Utility/Expected.h"
#include "Utility/Float.h"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <cstdint>
#include <Windows.h>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f16 = float16_t;
using f32 = float;
using f64 = double;

template<typename T>
using ExpectedHRes = Expected<T, HRESULT>;

#if _MSC_VER >= 1929 // VS2019 v16.10 and later (_MSC_FULL_VER >= 192829913 for VS 2019 v16.9)
// Works with /std:c++14 and /std:c++17, and performs optimization
#define NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
// no-op in MSVC v14x ABI
#define NO_UNIQUE_ADDRESS /* [[no_unique_address]] */
#endif
