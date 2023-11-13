#include "D3D12/CommandList.h"
#include "Platform.h"

#include <array>
#include <cstddef>

namespace RootSigConstantTypeTests
{
	template<u32 Size>
	struct TrivialType
	{
		std::array<std::byte, Size> Data;
	};

	template<u32 Size>
	struct NonTrivialType : TrivialType<Size>
	{
		NonTrivialType() {}
	};

	static_assert(RootSigConstantType<TrivialType<4>>);
	static_assert(!RootSigConstantType<TrivialType<3>>);
	static_assert(!RootSigConstantType<TrivialType<5>>);

	static_assert(!RootSigConstantType<NonTrivialType<4>>);
	static_assert(!RootSigConstantType<NonTrivialType<3>>);
	static_assert(!RootSigConstantType<NonTrivialType<5>>);
}