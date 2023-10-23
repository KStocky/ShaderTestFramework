#pragma once

#include <concepts>
#include <type_traits>

namespace ConceptsPrivate
{
	template<auto T>
	struct TestLiteralType {};
}

template<typename T, typename... Us>
concept IsAnyOf = (std::same_as<T, Us> || ...);

template<typename T>
concept EmptyType = std::is_empty_v<T>;

template<typename T>
concept FinalType = std::is_final_v<T>;

template<typename T>
concept PointerType = std::is_pointer_v<T>;

template<typename T>
concept DefaultConstructibleType = std::is_default_constructible_v<T>;

template<typename T>
concept MoveAssignableType = std::is_move_assignable_v<T>;

template<typename T>
concept MoveConstructibleType = std::is_move_constructible_v<T>;

template<typename T>
concept PureFunctionType = std::is_function_v<std::remove_pointer_t<std::remove_reference_t<T>>>;

template<typename T>
concept CapturelessLambdaType = !PureFunctionType<T> && requires(T InT)
{
	{+InT} -> PureFunctionType;
};

template<typename T>
concept NonTemplatedCallableType = requires()
{
	typename std::void_t<decltype(&std::decay_t<T>::operator() )>;
};

template<typename T>
concept ConstexprDefaultConstructableType = requires()
{
	typename ConceptsPrivate::TestLiteralType < T{} > ;
};

template<typename T, typename... U>
concept TemplatedCallableType = requires()
{
	typename std::void_t<decltype(&std::decay_t<T>::template operator() < U... > )>;
};

template<typename T, typename... U>
concept CallableType = (TemplatedCallableType<T, U...> || NonTemplatedCallableType<T>);

template<typename T, typename... U>
concept EmptyCallableType = std::is_empty_v<T> && CallableType<T, U...>;

template<typename T, typename... U>
concept ConstexprDefaultConstructableEmptyCallableType = ConstexprDefaultConstructableType<T> && EmptyCallableType<T, U...>;

