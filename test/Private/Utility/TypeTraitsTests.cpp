#include <Utility/TypeTraits.h>

namespace TUnqualifiedTests
{
    using namespace stf;
	static constexpr bool Identity = std::is_same_v<int, TUnqualified<int>>;
	static constexpr bool Pointer = std::is_same_v<int, TUnqualified<int*>>;
	static constexpr bool Const = std::is_same_v<int, TUnqualified<const int>>;
	static constexpr bool LValRef = std::is_same_v<int, TUnqualified<int&>>;
	static constexpr bool RValRef = std::is_same_v<int, TUnqualified<int&&>>;
	static constexpr bool ConstPointer = std::is_same_v<int, TUnqualified<int*const>>;
	static constexpr bool ConstPointerToConst = std::is_same_v<int, TUnqualified<const int* const>>;
	static constexpr bool ConstRef = std::is_same_v<int, TUnqualified<const int&>>;
	static constexpr bool LotsOfPointers = std::is_same_v<int, TUnqualified<const int**const*&>>;

	static_assert(Identity);
	static_assert(Pointer);
	static_assert(Const);
	static_assert(LValRef);
	static_assert(RValRef);
	static_assert(ConstPointer);
	static_assert(ConstPointerToConst);
	static_assert(ConstRef);
	static_assert(LotsOfPointers);
}

namespace TIsInstantiationOfTests
{
    using namespace stf;
	template<typename T, typename U>
	struct TestTemplate
	{};

	template<typename T, typename U>
	struct OtherTestTemplate
	{};

	template<typename T, T Val>
	struct NTTPTemplate
	{};

	using NTTPInstantiation = NTTPTemplate<int, 42>;

	static_assert(TIsInstantiationOf<TestTemplate, TestTemplate<int, double>>::Value);
	static_assert(!TIsInstantiationOf<OtherTestTemplate, TestTemplate<int, double>>::Value);

	static_assert(!TIsInstantiationOf<OtherTestTemplate, int>::Value);

	// This will not compile
	//static_assert(TIsInstantiationOf<NTTPTemplate, NTTPInstantiation>::Value);
}