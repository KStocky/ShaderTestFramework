#pragma once

#include "Utility/Concepts.h"
#include "Utility/TypeList.h"

namespace Private
{
	template<typename InFuncType>
	struct TFuncTraitsBase;

	template<typename RetType, typename... InParamTypes>
	struct TFuncTraitsBase< RetType(InParamTypes...)>
	{
		using ReturnType = RetType;
		using ParamTypes = TypeList<InParamTypes...>;
		using CallSignature = RetType(InParamTypes...);
		using ObjType = void;
		static constexpr bool IsConst = false;
	};

	template<typename RetType, typename InObjType, typename... InParamTypes>
	struct TFuncTraitsBase< RetType(InObjType::*)(InParamTypes...)>
	{
		using ReturnType = RetType;
		using ParamTypes = TypeList<InParamTypes...>;
		using CallSignature = RetType(InParamTypes...);
		using ObjType = InObjType;
		static constexpr bool IsConst = false;
	};

	template<typename RetType, typename InObjType, typename... InParamTypes>
	struct TFuncTraitsBase< RetType(InObjType::*)(InParamTypes...) const>
	{
		using ReturnType = RetType;
		using ParamTypes = TypeList<InParamTypes...>;
		using CallSignature = RetType(InParamTypes...);
		using ObjType = InObjType;
		static constexpr bool IsConst = true;
	};
}

template<typename InFuncType, typename... InArgs>
struct TFuncTraits;

template<typename InRetType, typename... InParamTypes>
struct TFuncTraits<InRetType(InParamTypes...)> : Private::TFuncTraitsBase<InRetType(InParamTypes...)> {};

template<NonTemplatedCallableType InCallableType>
struct TFuncTraits<InCallableType> : Private::TFuncTraitsBase<decltype(&std::decay_t<InCallableType>::operator())> {};

template<NonTemplatedCallableType InCallableType, typename... InArgs>
struct TFuncTraits<InCallableType, TypeList<InArgs...>> : Private::TFuncTraitsBase<decltype(&std::decay_t<InCallableType>::operator())> {};

template<typename InCallableType, typename... InArgs>
	requires TemplatedCallableType<InCallableType, InArgs...>
struct TFuncTraits<InCallableType, TypeList<InArgs...>> : Private::TFuncTraitsBase<decltype(&std::decay_t<InCallableType>::template operator() < InArgs... > )> {};

