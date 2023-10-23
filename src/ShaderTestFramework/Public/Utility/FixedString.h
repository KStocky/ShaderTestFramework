#pragma once

#include "Platform.h"
#include <algorithm>

template<u64 InSize>
struct FixedString
{
	char Data[InSize] = {};
	static constexpr auto Length = InSize - 1;

	constexpr FixedString(const char(&InString)[InSize])
	{
		std::copy(std::cbegin(InString), std::cend(InString), std::begin(Data));
	}
};

template<u64 InLength>
FixedString(const char(&)[InLength]) -> FixedString<InLength>;