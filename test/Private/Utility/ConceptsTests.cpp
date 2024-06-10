#include <Utility/Concepts.h>

#include <array>
#include <cstddef>

namespace CallableTypeTests
{
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
	using TemplatedLambdaParams = decltype([]<typename T>(float) {});
	using AutoLambdaNoExtraParams = decltype([](auto) {});
	using AutoLambdaExtraParams = decltype([](auto, float) {});

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
	using TemplatedLambdaParams = decltype([]<typename T>(float) {});
	using AutoLambdaNoExtraParams = decltype([](auto) {});
	using AutoLambdaExtraParams = decltype([](auto, float) {});

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
	using TemplatedLambdaParams = decltype([]<typename T>(float) {});
	using AutoLambdaNoExtraParams = decltype([](auto) {});
	using AutoLambdaExtraParams = decltype([](auto, float) {});

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

namespace HLSLTypeTriviallyConvertibleTypeTests
{
    template<std::size_t InAlignment>
    struct Trivial
    {
        std::array<std::byte, InAlignment> A;
    };

    template<std::size_t InAlignment>
    struct NotTrivial
    {
        NotTrivial(const NotTrivial&){}
        std::array<std::byte, InAlignment> A;
    };

    template<template<std::size_t> typename T, std::size_t InAlignment>
    struct alignas(InAlignment) NotFormattable
    {
        T<InAlignment> A;
    };

    template<template<std::size_t> typename T, std::size_t InAlignment>
    struct alignas(InAlignment) Formattable
    {
        T<InAlignment> A;
    };

    using TrivialNotValidAlignmentNotFormattable = NotFormattable<Trivial, 1>;
    using Trivial4ByteAlignedNotFormattable = NotFormattable<Trivial, 4>;
    using NotTrivialNotValidAlignmentNotFormattable = NotFormattable<NotTrivial, 1>;
    using NotTrivial4ByteAlignedNotFormattable = NotFormattable<NotTrivial, 4>;

    using TrivialNotValidAlignmentFormattable = Formattable<Trivial, 1>;
    using Trivial4ByteAlignedFormattable = Formattable<Trivial, 4>;
    using NotTrivialNotValidAlignmentFormattable = Formattable<NotTrivial, 1>;
    using NotTrivial4ByteAlignedFormattable = Formattable<NotTrivial, 4>;
}

template <template<std::size_t> typename T, std::size_t InAlignment>
struct std::formatter<HLSLTypeTriviallyConvertibleTypeTests::Formattable<T, InAlignment>> : std::formatter<int> {
    auto format(const HLSLTypeTriviallyConvertibleTypeTests::Formattable<T, InAlignment>&, auto& ctx) const {
        return std::formatter<int>::format(42, ctx);
    }
};

namespace HLSLTypeTriviallyConvertibleTypeTests
{
    static_assert(!std::is_trivially_copyable_v<NotTrivial<4>>);
    static_assert(std::is_trivially_copyable_v<Trivial<4>>);

    static_assert(!HLSLTypeTriviallyConvertibleType<TrivialNotValidAlignmentNotFormattable>);
    static_assert(!HLSLTypeTriviallyConvertibleType<Trivial4ByteAlignedNotFormattable>);
    static_assert(!HLSLTypeTriviallyConvertibleType<NotTrivialNotValidAlignmentNotFormattable>);
    static_assert(!HLSLTypeTriviallyConvertibleType<NotTrivial4ByteAlignedNotFormattable>);

    static_assert(!HLSLTypeTriviallyConvertibleType<TrivialNotValidAlignmentFormattable>);
    static_assert(!HLSLTypeTriviallyConvertibleType<NotTrivialNotValidAlignmentFormattable>);
    static_assert(!HLSLTypeTriviallyConvertibleType<NotTrivial4ByteAlignedFormattable>);

    static_assert(HLSLTypeTriviallyConvertibleType<Trivial4ByteAlignedFormattable>);
}

namespace InstantiatableFromTests
{
    struct A {};
    struct B {};

    template<typename T>
    struct OneTypeParam {};

    template<typename T, typename U>
    struct TwoTypeParam {};

    template<typename T>
        requires (!std::same_as<T, A>)
    struct NotA;

    template<typename T>
    struct NotB;

    template<>
    struct NotB<A> {};


    static_assert(InstantiatableFrom<OneTypeParam, A>);
    static_assert(InstantiatableFrom<OneTypeParam, B>);
    static_assert(!InstantiatableFrom<OneTypeParam, A, B>);
    static_assert(!InstantiatableFrom<OneTypeParam, A, B>);

    static_assert(!InstantiatableFrom<TwoTypeParam, A>);
    static_assert(!InstantiatableFrom<TwoTypeParam, B>);
    static_assert(InstantiatableFrom<TwoTypeParam, A, B>);
    static_assert(InstantiatableFrom<TwoTypeParam, A, B>);

    static_assert(!InstantiatableFrom<NotA, A>);
    static_assert(InstantiatableFrom<NotA, B>);

    static_assert(InstantiatableFrom<NotB, A>);
}

namespace NewableTests
{
    struct NewableStruct {};

    struct NotNewableStruct
    {
        static void* operator new(std::size_t) = delete;
    };

    static_assert(Newable<NewableStruct>);
    static_assert(!Newable<NotNewableStruct>);
}