#pragma once

#include <concepts>
#include <type_traits>
export module Engine.Core.Concepts;

namespace ConceptsPrivate
{
	template<auto T>
	struct TestLiteralType {};
}

export namespace Engine
{
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
}

module :private;

namespace CallableTypeTests
{
	using namespace Engine;
	static auto LambdaFunc = [](float) {return 42; };

	static auto LambdaFuncWithCapture = [ret = 42](float) {return ret; };

	struct EmptyCallable
	{
		void operator()() {}
	};

	struct EmptyCallableWithUnaryPlus
	{
		void operator()() {}
		void operator+() {}
	};

	void Function() {}

	struct MyStruct
	{
		static void Function()
		{}
	};

	struct TemplatedCallableNoParams
	{
		template<typename T>
		void operator()() {}
	};

	struct TemplatedCallableParams
	{
		template<typename T>
		void operator()(T In, float Other) {}
	};

	struct AutoCallableNoExtraParams
	{
		void operator()(auto In) {}
	};

	struct AutoCallableExtraParams
	{
		void operator()(auto In, float Other) {}
	};

	using TemplatedLambdaNoParams = decltype([]<typename T>() {});
	using TemplatedLambdaParams = decltype([]<typename T>(float In) {});
	using AutoLambdaNoExtraParams = decltype([](auto In) {});
	using AutoLambdaExtraParams = decltype([](auto In, float Other) {});

	static_assert(CallableType<decltype(LambdaFunc)>);
	static_assert(CallableType<decltype(LambdaFuncWithCapture)>);
	static_assert(CallableType<EmptyCallable>);
	static_assert(CallableType<EmptyCallableWithUnaryPlus>);
	static_assert(!CallableType<decltype(Function)>);
	static_assert(!CallableType<decltype(MyStruct::Function)>);
	static_assert(CallableType<TemplatedCallableNoParams, int>);
	static_assert(CallableType<TemplatedCallableParams, int>);
	static_assert(CallableType<AutoCallableNoExtraParams, int>);
	static_assert(CallableType<AutoCallableExtraParams, int>);
	static_assert(CallableType<TemplatedLambdaNoParams, int>);
	static_assert(CallableType<TemplatedLambdaParams, int>);
	static_assert(CallableType<AutoLambdaNoExtraParams, int>);
	static_assert(CallableType<AutoLambdaExtraParams, int>);
}

namespace TemplatedCallableTypeTests
{
	using namespace Engine;
	static auto LambdaFunc = [](float) {return 42; };

	static auto LambdaFuncWithCapture = [ret = 42](float) {return ret; };

	struct EmptyCallable
	{
		void operator()() {}
	};

	struct EmptyCallableWithUnaryPlus
	{
		void operator()() {}
		void operator+() {}
	};

	void Function() {}

	struct MyStruct
	{
		static void Function()
		{}
	};

	struct TemplatedCallableNoParams
	{
		template<typename T>
		void operator()() {}
	};

	struct TemplatedCallableParams
	{
		template<typename T>
		void operator()(T In, float Other) {}
	};

	struct AutoCallableNoExtraParams
	{
		void operator()(auto In) {}
	};

	struct AutoCallableExtraParams
	{
		void operator()(auto In, float Other) {}
	};

	using TemplatedLambdaNoParams = decltype([]<typename T>() {});
	using TemplatedLambdaParams = decltype([]<typename T>(float In) {});
	using AutoLambdaNoExtraParams = decltype([](auto In) {});
	using AutoLambdaExtraParams = decltype([](auto In, float Other) {});

	static_assert(!TemplatedCallableType<decltype(LambdaFunc)>);
	static_assert(!TemplatedCallableType<decltype(LambdaFuncWithCapture)>);
	static_assert(!TemplatedCallableType<EmptyCallable>);
	static_assert(!TemplatedCallableType<EmptyCallableWithUnaryPlus>);
	static_assert(!TemplatedCallableType<decltype(Function)>);
	static_assert(!TemplatedCallableType<decltype(MyStruct::Function)>);
	static_assert(TemplatedCallableType<TemplatedCallableNoParams, int>);
	static_assert(TemplatedCallableType<TemplatedCallableParams, int>);
	static_assert(TemplatedCallableType<AutoCallableNoExtraParams, int>);
	static_assert(TemplatedCallableType<AutoCallableExtraParams, int>);
	static_assert(TemplatedCallableType<TemplatedLambdaNoParams, int>);
	static_assert(TemplatedCallableType<TemplatedLambdaParams, int>);
	static_assert(TemplatedCallableType<AutoLambdaNoExtraParams, int>);
	static_assert(TemplatedCallableType<AutoLambdaExtraParams, int>);
}

namespace NonTemplatedCallableTypeTests
{
	using namespace Engine;
	static auto LambdaFunc = [](float) {return 42; };

	static auto LambdaFuncWithCapture = [ret = 42](float) {return ret; };

	struct EmptyCallable
	{
		void operator()() {}
	};

	struct EmptyCallableWithUnaryPlus
	{
		void operator()() {}
		void operator+() {}
	};

	void Function() {}

	struct MyStruct
	{
		static void Function()
		{}
	};

	struct TemplatedCallableNoParams
	{
		template<typename T>
		void operator()() {}
	};

	struct TemplatedCallableParams
	{
		template<typename T>
		void operator()(T In, float Other) {}
	};

	struct AutoCallableNoExtraParams
	{
		void operator()(auto In) {}
	};

	struct AutoCallableExtraParams
	{
		void operator()(auto In, float Other) {}
	};

	using TemplatedLambdaNoParams = decltype([]<typename T>() {});
	using TemplatedLambdaParams = decltype([]<typename T>(float In) {});
	using AutoLambdaNoExtraParams = decltype([](auto In) {});
	using AutoLambdaExtraParams = decltype([](auto In, float Other) {});

	static_assert(NonTemplatedCallableType<decltype(LambdaFunc)>);
	static_assert(NonTemplatedCallableType<decltype(LambdaFuncWithCapture)>);
	static_assert(NonTemplatedCallableType<EmptyCallable>);
	static_assert(NonTemplatedCallableType<EmptyCallableWithUnaryPlus>);
	static_assert(!NonTemplatedCallableType<decltype(Function)>);
	static_assert(!NonTemplatedCallableType<decltype(MyStruct::Function)>);
	static_assert(!NonTemplatedCallableType<TemplatedCallableNoParams>);
	static_assert(!NonTemplatedCallableType<TemplatedCallableParams>);
	static_assert(!NonTemplatedCallableType<AutoCallableNoExtraParams>);
	static_assert(!NonTemplatedCallableType<AutoCallableExtraParams>);
	static_assert(!NonTemplatedCallableType<TemplatedLambdaNoParams>);
	static_assert(!NonTemplatedCallableType<TemplatedLambdaParams>);
	static_assert(!NonTemplatedCallableType<AutoLambdaNoExtraParams>);
	static_assert(!NonTemplatedCallableType<AutoLambdaExtraParams>);
}

namespace PureFunctionTests
{
	using namespace Engine;
	static auto LambdaFunc = [](float) {return 42; };

	static auto LambdaFuncWithCapture = [ret = 42](float) {return ret; };

	struct EmptyCallable
	{
		void operator()() {}
	};

	struct EmptyCallableWithUnaryPlus
	{
		void operator()() {}
		void operator+() {}
	};

	void Function() {}

	struct MyStruct
	{
		static void Function()
		{}
	};

	static_assert(!PureFunctionType<decltype(LambdaFunc)>);
	static_assert(!PureFunctionType<decltype(LambdaFuncWithCapture)>);
	static_assert(!PureFunctionType<EmptyCallable>);
	static_assert(!PureFunctionType<EmptyCallableWithUnaryPlus>);
	static_assert(PureFunctionType<decltype(Function)>);
	static_assert(PureFunctionType<decltype(MyStruct::Function)>);
}

namespace CapturelessLambdaTests
{
	using namespace Engine;
	static auto LambdaFunc = [](float) {return 42; };

	static auto LambdaFuncWithCapture = [ret = 42](float) {return ret; };

	struct EmptyCallable
	{
		void operator()() {}
	};

	struct EmptyCallableWithUnaryPlus
	{
		void operator()() {}
		void operator+() {}
	};

	void Function() {}

	struct MyStruct
	{
		static void Function()
		{}
	};

	static_assert(CapturelessLambdaType<decltype(LambdaFunc)>);
	static_assert(!CapturelessLambdaType<decltype(LambdaFuncWithCapture)>);
	static_assert(!CapturelessLambdaType<EmptyCallable>);
	static_assert(!CapturelessLambdaType<EmptyCallableWithUnaryPlus>);
	static_assert(!CapturelessLambdaType<decltype(Function)>);
	static_assert(!CapturelessLambdaType<decltype(MyStruct::Function)>);
}

namespace ConstexprDefaultConstructableTypeTests
{
	struct ConstexprDefaultConstructable
	{
		constexpr ConstexprDefaultConstructable() {}
	};

	struct NotConstexprDefaultConstructable
	{
		NotConstexprDefaultConstructable() {}
	};

	struct ConstexprNonDefaultConstructable
	{
		ConstexprNonDefaultConstructable() {}
		constexpr ConstexprNonDefaultConstructable(int){}
	};

	static_assert(ConstexprDefaultConstructableType<ConstexprDefaultConstructable>);
	static_assert(!ConstexprDefaultConstructableType<NotConstexprDefaultConstructable>);
	static_assert(!ConstexprDefaultConstructableType<ConstexprNonDefaultConstructable>);
}