
#include <Utility/Type.h>
#include <Utility/TypeList.h>

namespace TypeListTests
{
    using namespace stf;
	using TypeList1 = TypeList<int, float, double>;
	using TypeList2 = TypeList<double, char, int, short>;
	using TypeList3 = TypeList<int&, const float&>;
	using TypeList4 = TypeList<bool, short, int&, int>;
	using TaggedList1 = TypeList
		<
		TaggedType<int, "MyInt">,
		TaggedType<float, "MyFloat">
		>;
	using TestPopBack = TypeList1::PopBack;
	using TestPushBack = TypeList1::PushBack<bool>;
	using TestInsertAtStart = TypeList1::InsertAt<0, bool>;
	using TestInsertAtEnd = TypeList1::InsertAt<TypeList1::Size, bool>;
	using TestInsertInMiddle = TypeList1::InsertAt<TypeList1::Size / 2, bool>;
	using TestRemoveAtStart = TypeList1::RemoveAt<0>;
	using TestRemoveAtEnd = TypeList1::RemoveAt<TypeList1::Size - 1>;
	using TestRemoveInMiddle = TypeList1::RemoveAt<1>;
	using TestDefaultSorted = TypeList2::Sort<>;
	using TestTypeNameLengthSort = TypeList2::Sort < []<typename T>() { return TypeToString<T>(); }, [](std::string_view InA, std::string_view InB) { return InA.size() < InB.size(); } > ;
	using TestTypeNameAlphaSort = TypeList2::Sort < []<typename T>() { return TypeToString<T>(); }, [](std::string_view InA, std::string_view InB) { return InA < InB; } > ;
	using TestRemoveRange = TypeList1::RemoveRange<0, 2>;

	static constexpr auto SizeOfTypeToKey = []<typename T>() { return sizeof(T); };
	static constexpr auto GreaterThan = [](std::size_t InA, std::size_t InB) { return InA > InB; };

	template<typename InTypeList, typename InIndices>
	concept SwizzleTester = requires()
	{
		typename InTypeList::template Swizzle<InIndices>;
	};
	using TestSorted = TypeList2::Sort<>;
	using TestUnsortedIndices = TypeList2::UnsortedIndices<>;
	using TestUnsortedSwizzle = TestSorted::Swizzle<TestUnsortedIndices>;
	static_assert(SwizzleTester<TypeList1, std::index_sequence<0, 1, 2>>);
	static_assert(SwizzleTester<TypeList1, std::index_sequence<0, 0, 0>>);
	static_assert(SwizzleTester<TypeList1, std::index_sequence<2, 0, 1>>);
	static_assert(!SwizzleTester<TypeList1, std::index_sequence<0, 1, 2, 3>>);
	static_assert(!SwizzleTester<TypeList1, std::index_sequence<0, 1>>);
	static_assert(SwizzleTester<TypeList<>, std::index_sequence<>>);

	template<typename InTypeList, template<typename, typename...>typename InTransformer, typename... InTransformerArgs>
	concept TransformTester = requires()
	{
		typename InTypeList::template Transform<InTransformer, InTransformerArgs...>;
	};

	template<typename T>
	struct InvalidTransformer
	{};

	template<typename T>
	struct IdentityTransformer
	{
		using Type = T;
	};

	template<typename T, typename CompareType>
	struct TransformerWithArgs
	{
		using Type = std::conditional_t<std::is_same_v<T, CompareType>, void, T>;
	};

	static_assert(!TransformTester<TypeList1, InvalidTransformer>);
	static_assert(TransformTester<TypeList1, IdentityTransformer>);
	static_assert(TransformTester<TypeList1, TransformerWithArgs, float>);

	template<typename T>
	struct AllIntTransformer
	{
		using Type = int;
	};

	static_assert(std::is_same_v<TypeList1::Transform<AllIntTransformer>, TypeList<int, int, int>>);
	static_assert(std::is_same_v<TypeList1::Transform<TransformerWithArgs, float>, TypeList<int, void, double>>);

	using TestGetByName = TaggedList1::TypeName<"MyFloat">;

	static_assert(std::is_same_v<TestGetByName, float>);

	static_assert(std::string_view("int") == TypeToString<int>());


	static_assert(std::is_same_v<TypeList1::Type<1>, float>);
	static_assert(std::is_same_v<TypeList<int, float>, TestPopBack>);
	static_assert(std::is_same_v<TypeList<int, float, double, bool>, TestPushBack>);
	static_assert(std::is_same_v<TypeList<bool, int, float, double>, TestInsertAtStart>);
	static_assert(std::is_same_v<TypeList<int, float, double, bool>, TestInsertAtEnd>);
	static_assert(std::is_same_v<TypeList<int, bool, float, double>, TestInsertInMiddle>);
	static_assert(std::is_same_v<TypeList<float, double>, TestRemoveAtStart>);
	static_assert(std::is_same_v<TypeList<int, float>, TestRemoveAtEnd>);
	static_assert(std::is_same_v<TypeList<int, double>, TestRemoveInMiddle>);
	static_assert(std::is_same_v<TypeList<char, short, int, double>, TestDefaultSorted>);
	static_assert(std::is_same_v<TypeList<int, char, short, double>, TestTypeNameLengthSort>);
	static_assert(std::is_same_v<TypeList<char, double, int, short>, TestTypeNameAlphaSort>);
	static_assert(std::is_same_v<TypeList<double>, TestRemoveRange>);
	static_assert(TestUnsortedIndices::size() == 4);
	static_assert(std::is_same_v<std::index_sequence<3, 0, 2, 1>, TestUnsortedIndices>);
	static_assert(std::is_same_v<typename TypeList<const int>::template Type<0>, const int>);
	
	static_assert(TypeList1::Equals(TypeList<int, float, double>{}));
	static_assert(!TypeList1::Equals(TypeList<int, float>{}));
	static_assert(!TypeList1::Equals(TypeList<int, double, float>{}));
	
	static_assert(TypeList1::AllOf( []<typename T>() { return sizeof(T) > 2; } ) );
	static_assert(!TypeList1::AllOf ( []<typename T>() { return sizeof(T) > 4; } ) );
	static_assert(TypeList3::AllOf ( []<typename T>() { return std::is_reference_v<T>; } ) );

	static_assert(TypeList1::AnyOf ( []<typename T>() { return sizeof(T) > 2; } ) );
	static_assert(TypeList1::AnyOf ( []<typename T>() { return sizeof(T) > 4; } ) );
	static_assert(TypeList4::AnyOf ( []<typename T>() { return std::is_reference_v<T>; } ) );
	static_assert(!TypeList1::AnyOf ( []<typename T>() { return std::is_reference_v<T>; } ) );

	using List = TypeList<double, bool>;
	using LargerList = List::PushBack<int>::PushBack<char>;
	static_assert(std::is_same_v<TypeList<double, bool, int, char>, LargerList>);

	using SlightlyLargerList = LargerList::InsertAt<1, short>;
	static_assert(std::is_same_v<TypeList<double, short, bool, int, char>, SlightlyLargerList>);

	using FinalList = SlightlyLargerList::RemoveAt<2>;
	static_assert(std::is_same_v<TypeList<double, short, int, char>, FinalList>);

	using SortedSmallestToLargest = FinalList::Sort<>;
	static_assert(std::is_same_v<TypeList<char, short, int, double>, SortedSmallestToLargest>);

	using SortedLargestToSmallest = FinalList::Sort <
		SizeOfTypeToKey,
		GreaterThan > ;
	static_assert(std::is_same_v<TypeList<double, int, short, char>, SortedLargestToSmallest>);

	using LargestToSmallestUnsortedIndices = FinalList::UnsortedIndices<
		SizeOfTypeToKey,
		GreaterThan
	>;
	static_assert(std::is_same_v<FinalList, SortedLargestToSmallest::Swizzle<LargestToSmallestUnsortedIndices>>);

	using SortedAlphabetically = FinalList::Sort <
		[]<typename T>() { return TypeToString<T>(); },
		[](std::string_view InA, std::string_view InB) { return InA < InB; } > ;
	static_assert(std::is_same_v<TypeList<char, double, int, short>, SortedAlphabetically>);

	using SortedSmallestNameFirst = FinalList::Sort <
		[]<typename T>() { return TypeToString<T>(); },
		[](std::string_view InA, std::string_view InB) { return InA.size() < InB.size(); } > ;
	static_assert(std::is_same_v<TypeList<int, char, short, double>, SortedSmallestNameFirst>);
}