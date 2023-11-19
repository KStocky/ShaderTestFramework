#pragma once
#include "Platform.h"
#include "Utility/FixedString.h"
#include "Utility/TypeTraits.h"

#include <algorithm>
#include <array>
#include <concepts>
#include <string_view>
#include <type_traits>

namespace Private
{
	template<auto I>
	using IndexIdentity = std::integral_constant<decltype(I), I>;

	template<auto... InIndexes>
	struct ConstantList {};

	template<typename... InTypes>
	struct TypeParamList {};

	template<typename T, auto InIndex, auto InSecondIndex = InIndex>
	class TypeListElem
	{
	public:
		using Type = T;
		static std::type_identity<T> DeclElem(IndexIdentity<InIndex>);
		static T DeclElemSecond(IndexIdentity<InSecondIndex>);
	};

	template<typename IndexSeq, typename SecondIndexSeq, class... InTypes>
	class TypeListBase;

	template<auto... InIndices, typename... InConstantListTypes, class... InTypes>
	class TypeListBase<std::index_sequence<InIndices...>, TypeParamList<InConstantListTypes...>, InTypes...>
		: TypeListElem<typename InTypes::Type, InIndices, InConstantListTypes::Value>...
	{
	public:

		using TypeListElem<typename InTypes::Type, InIndices, InConstantListTypes::Value>::DeclElem...;
		using TypeListElem<typename InTypes::Type, InIndices, InConstantListTypes::Value>::DeclElemSecond...;
	};

	template<auto... InIndices, auto... InSecondIndices, class... InTypes>
	class TypeListBase<std::index_sequence<InIndices...>, std::index_sequence<InSecondIndices...>, InTypes...>
		: TypeListElem<InTypes, InIndices, InSecondIndices>...
	{
	public:

		using TypeListElem<InTypes, InIndices, InSecondIndices>::DeclElem...;
		using TypeListElem<InTypes, InIndices, InSecondIndices>::DeclElemSecond...;
	};

	template<>
	class TypeListBase<std::index_sequence<>, std::index_sequence<>>
	{
	public:

		static void DeclElem(IndexIdentity<0>);
	};

	template<auto InTypeToKeyLambda, auto InKeyCompareLambda>
	concept SortableListFuncs = requires()
	{
		{InKeyCompareLambda(InTypeToKeyLambda.template operator() < i32 > (), InTypeToKeyLambda.template operator() < i32 > ())} -> std::same_as<bool>;
	};

	// These are here and not in TypeList because of this bug 
	// https://developercommunity.visualstudio.com/t/Internal-Compiler-Error-when-class-templ/10322890?port=1025&fsid=8e36d308-ce0a-48a7-a57c-848fde6f9d49&ref=native&refTime=1679846442685&refUserId=d2d4eec3-3cb8-454b-a663-fd1ffab50076
	// When this is fixed we can move these back to TypeList.
	static constexpr auto DefaultTypeToKeyFunc = []<typename T>() { return sizeof(T); };
	static constexpr auto DefaultKeyCompareFunc = [](u64 InA, u64 InB) {return InA < InB; };
	static constexpr auto DefaultCompareListFunc = []<typename T, typename U>() { return std::is_same_v<T, U>; };

	template<template<typename, typename...> typename T, typename... InArgTypes>
	concept TypeTransformer = requires()
	{
		typename T<i32, InArgTypes...>::Type;
	};
}

template<typename InType, FixedString InVal>
struct TaggedType
{
	using Type = InType;
	static constexpr auto Value = InVal;
};

template<typename T>
struct IsTaggedTypeInstantiation
{
	static constexpr bool Value = false;
};

template<typename T, FixedString V>
struct IsTaggedTypeInstantiation<TaggedType<T, V>>
{
	static constexpr bool Value = true;
};

template<typename... InTypes>
class TypeList;

template<typename T>
concept TypeListType = TIsInstantiationOf<TypeList, T>::Value;

template<typename... InTypes>
class TypeList
	:
	std::conditional_t<
	(IsTaggedTypeInstantiation<InTypes>::Value && ...),
	Private::TypeListBase<std::index_sequence_for<InTypes...>, Private::TypeParamList<InTypes...>, InTypes...>,
	Private::TypeListBase<std::index_sequence_for<InTypes...>, std::index_sequence_for<InTypes...>, InTypes...>
	>
{
	template<typename Indices>
	struct PopBackHelper;

	template<u64... Indices>
	struct PopBackHelper<std::index_sequence<Indices...>>
	{
		using Type = TypeList<typename TypeList<InTypes...>::template Type<Indices>...>;
	};

	template<u64 InIndex, typename BeforeIndices, typename AfterIndices, typename Type>
	struct InsertHelper;

	template<u64 InIndex, u64... InBeforeIndices, u64... InAfterIndices, typename InsertedType>
	struct InsertHelper<InIndex, std::index_sequence<InBeforeIndices...>, std::index_sequence<InAfterIndices...>, InsertedType>
	{
		using CurrList = TypeList<InTypes...>;
		using Type = TypeList<typename CurrList::template Type<InBeforeIndices>..., InsertedType, typename CurrList::template Type<(InIndex + InAfterIndices)>...>;
	};

	template<u64 InIndex, u64 InNum, typename BeforeIndices, typename AfterIndices>
	struct RemoveHelper;

	template<u64 InIndex, u64 InNum, u64... BeforeIndices, u64... AfterIndices>
	struct RemoveHelper<InIndex, InNum, std::index_sequence<BeforeIndices...>, std::index_sequence<AfterIndices...>>
	{
		using CurrList = TypeList<InTypes...>;
		using Type = TypeList<typename CurrList::template Type<BeforeIndices>..., typename CurrList::template Type<InIndex + InNum + AfterIndices>...>;
	};

	using BaseType = std::conditional_t<
		(sizeof...(InTypes) > 0) && (IsTaggedTypeInstantiation<InTypes>::Value && ...),
		Private::TypeListBase<std::index_sequence_for<InTypes...>, Private::TypeParamList<InTypes...>, InTypes...>,
		Private::TypeListBase<std::index_sequence_for<InTypes...>, std::index_sequence_for<InTypes...>, InTypes...>
	>;

	template<typename InIndicesToSort>
	struct SortIndicesHelper;

	template<u64... InIndicesToSort>
	struct SortIndicesHelper<std::index_sequence<InIndicesToSort...>>
	{
		template<u64... InIndices>
		static constexpr auto SortIndices(std::index_sequence<InIndices...>)
		{
			std::array unsorted{ InIndices... };
			std::array vars{ InIndicesToSort... };
			std::ranges::sort(unsorted,
				[vars](u64 InA, u64 InB)
				{
					return vars[InA] < vars[InB];
				});
			return unsorted;
		}

		template<u64... InIndices>
			requires (sizeof...(InIndices) > 0)
		static constexpr auto GetSortedIndices(std::index_sequence<InIndices...>)
		{
			constexpr auto sorted = SortIndices(std::index_sequence<InIndices...>{});
			return std::index_sequence<sorted[InIndices]...>{};
		}

		template<u64... InIndices>
			requires (sizeof...(InIndices) == 0)
		static constexpr auto GetSortedIndices(std::index_sequence<InIndices...>)
		{
			return std::index_sequence<>{};
		}

		using Type = decltype(GetSortedIndices(std::make_index_sequence<sizeof...(InIndicesToSort)>{}));
	};

	template<auto TypeToKeyFunc, auto CompareFunc>
	struct SortHelper
	{
		template<u64... InIndices>
		static constexpr auto SortIndices(std::index_sequence<InIndices...>)
		{
			std::array unsorted{ InIndices... };
			std::array keyArray
			{
				TypeToKeyFunc.template operator() < InTypes > ()...
			};
			std::ranges::sort(unsorted,
				[keyArray](u64 InA, u64 InB)
				{
					return CompareFunc(keyArray[InA], keyArray[InB]);
				});
			return unsorted;
		}

		template<u64... InIndices>
			requires (sizeof...(InIndices) > 0)
		static constexpr auto SortTypes(std::index_sequence<InIndices...> In)
		{
			constexpr auto sorted = SortIndices(In);
			return TypeList<
				TypeList<InTypes...>::template Type<sorted[InIndices]>...
			>{};
		}

		template<u64... InIndices>
			requires (sizeof...(InIndices) == 0)
		static constexpr auto SortTypes(std::index_sequence<InIndices...>)
		{
			return TypeList<>{};
		}

		template<u64... InIndices>
			requires (sizeof...(InIndices) > 0)
		static constexpr auto GetSortedIndices(std::index_sequence<InIndices...> In)
		{
			constexpr auto sorted = SortIndices(In);
			return std::index_sequence<sorted[InIndices]...>{};
		}

		template<u64... InIndices>
			requires (sizeof...(InIndices) == 0)
		static constexpr auto GetSortedIndices(std::index_sequence<InIndices...>)
		{
			return std::index_sequence<>{};
		}

		using Type = decltype(SortTypes(std::index_sequence_for<InTypes...>{}));

		using IndicesType = decltype(GetSortedIndices(std::index_sequence_for<InTypes...>{}));

		using UnsortedIndicesType = typename SortIndicesHelper<IndicesType>::Type;
	};

	template<typename InIndices>
	struct SwizzleValidator
	{
		static_assert(AlwaysFalse<InIndices>, "Did not provide a valid index sequence. Please ensure that the type passed in was a valid std::index_sequence");
	};

	template<u64... InIndices>
	struct SwizzleValidator<std::index_sequence<InIndices...>>
	{
		static constexpr bool IsValidIndexSequence()
		{
			if constexpr (sizeof...(InIndices) == 0)
			{
				return true;
			}
			else
			{
				if constexpr (sizeof...(InIndices) != sizeof...(InTypes))
				{
					return false;
				}

				std::array indices{ InIndices... };
				return std::ranges::all_of(indices, [](u64 InIndex) { return InIndex < sizeof...(InTypes); });
			}
		}

		static constexpr bool Valid = IsValidIndexSequence();
	};

	template<typename InIndices>
	struct SwizzleHelper;

	template<u64... InIndices>
		requires (sizeof...(InIndices) > 0 && SwizzleValidator<std::index_sequence<InIndices...>>::Valid)
	struct SwizzleHelper<std::index_sequence<InIndices...>>
	{
		using Type = TypeList<
			typename TypeList<InTypes...>::template Type<InIndices>...
		>;
	};

	template<u64... InIndices>
		requires (sizeof...(InIndices) == 0 && SwizzleValidator<std::index_sequence<InIndices...>>::Valid)
	struct SwizzleHelper<std::index_sequence<InIndices...>>
	{
		using Type = TypeList<>;
	};

public:

	static constexpr u64 Size = sizeof...(InTypes);

	template<u64 InIndex>
		requires (InIndex < Size)
	using Type = typename decltype(BaseType::DeclElem(std::declval<Private::IndexIdentity<InIndex>>()))::type;

	template<FixedString InName>
	using TypeName = decltype(BaseType::DeclElemSecond(std::declval<Private::IndexIdentity<InName>>()));

	template<typename T>
	using PushBack = TypeList<InTypes..., T>;

	using PopBack = PopBackHelper<std::make_index_sequence<(Size > 0) ? Size - 1 : 0>>::Type;

	template<u64 InIndex, typename T>
		requires (InIndex <= Size)
	using InsertAt = InsertHelper<InIndex, std::make_index_sequence<InIndex>, std::make_index_sequence<Size - InIndex>, T>::Type;

	template<u64 InIndex>
		requires (InIndex < Size)
	using RemoveAt = RemoveHelper<InIndex, 1, std::make_index_sequence<InIndex>, std::make_index_sequence<Size - (InIndex + 1)>>::Type;

	template<u64 InIndex, u64 InNum>
		requires ((InIndex + InNum) <= Size)
	using RemoveRange = RemoveHelper<InIndex, InNum, std::make_index_sequence<InIndex>, std::make_index_sequence<Size - (InIndex + InNum)>>::Type;

	template<auto TypeToKeyFunc = Private::DefaultTypeToKeyFunc, auto CompareFunc = Private::DefaultKeyCompareFunc>
		requires Private::SortableListFuncs<TypeToKeyFunc, CompareFunc>
	using Sort = SortHelper<TypeToKeyFunc, CompareFunc>::Type;

	template<auto TypeToKeyFunc = Private::DefaultTypeToKeyFunc, auto CompareFunc = Private::DefaultKeyCompareFunc>
		requires Private::SortableListFuncs<TypeToKeyFunc, CompareFunc>
	using SortedIndices = SortHelper<TypeToKeyFunc, CompareFunc>::IndicesType;

	template<auto TypeToKeyFunc = Private::DefaultTypeToKeyFunc, auto CompareFunc = Private::DefaultKeyCompareFunc>
		requires Private::SortableListFuncs<TypeToKeyFunc, CompareFunc>
	using UnsortedIndices = SortHelper<TypeToKeyFunc, CompareFunc>::UnsortedIndicesType;

	template<typename InIndexSequence>
	using Swizzle = SwizzleHelper<InIndexSequence>::Type;

	static consteval auto AllOf(auto InFunc)
	{
		return (InFunc.template operator() < InTypes > () && ...);
	}

	template<typename... InOtherTypes>
		requires (sizeof...(InTypes) != sizeof...(InOtherTypes))
	static consteval auto AllOf(auto InFunc, TypeList<InOtherTypes...>)
	{
		return false;
	}

	template<typename... InOtherTypes>
		requires (sizeof...(InTypes) == sizeof...(InOtherTypes))
	static consteval auto AllOf(auto InFunc, TypeList<InOtherTypes...>)
	{
		return (InFunc.template operator() < InTypes, InOtherTypes > () && ...);
	}

	static consteval auto AnyOf(auto InFunc)
	{
		return (InFunc.template operator() < InTypes > () || ...);
	}

	template<typename... InOtherTypes>
		requires (sizeof...(InTypes) != sizeof...(InOtherTypes))
	static consteval auto AnyOf(auto InFunc, TypeList<InOtherTypes...>)
	{
		return false;
	}

	template<typename... InOtherTypes>
		requires (sizeof...(InTypes) == sizeof...(InOtherTypes))
	static consteval auto AnyOf(auto InFunc, TypeList<InOtherTypes...>)
	{
		return (InFunc.template operator() < InTypes, InOtherTypes > () || ...);
	}

	template<typename... InOtherTypes>
	static consteval auto Equals(TypeList<InOtherTypes...> In)
	{
		return AllOf([]<typename T, typename U>() { return std::is_same_v<T, U>; }, In);
	}

	template<template<typename, typename...>typename InTransformer, typename... InArgs>
		requires Private::TypeTransformer<InTransformer, InArgs...>
	using Transform = TypeList<typename InTransformer<InTypes, InArgs...>::Type...>;
};