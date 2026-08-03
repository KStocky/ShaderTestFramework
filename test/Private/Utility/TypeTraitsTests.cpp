#include <Utility/TypeTraits.h>

#include <concepts>

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

	static_assert(TIsInstantiationOf<TestTemplate<int, double>, TestTemplate>::Value);
	static_assert(!TIsInstantiationOf<TestTemplate<int, double>, OtherTestTemplate>::Value);

	static_assert(!TIsInstantiationOf<int, OtherTestTemplate>::Value);

	// This will not compile
	//static_assert(TIsInstantiationOf<NTTPTemplate, NTTPInstantiation>::Value);
}

namespace stf::TFirstTypeTests
{
    template<typename... Ts>
    concept CValidateTFirstType = requires
    {
        typename TFirstType<Ts...>;
    };

    struct A {};
    struct B {};

    static_assert(!CValidateTFirstType<>);
    static_assert(CValidateTFirstType<A>);
    static_assert(CValidateTFirstType<A, A, A, A>);
    static_assert(CValidateTFirstType<B, A, B, A>);

    static_assert(std::same_as<A, TFirstType<A>>);
    static_assert(!std::same_as<B, TFirstType<A>>);
    static_assert(!std::same_as<B, TFirstType<A, A, B>>);
    static_assert(std::same_as<B, TFirstType<B, A, A, B>>);
}