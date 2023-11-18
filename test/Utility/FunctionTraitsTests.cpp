#include <Utility/FunctionTraits.h>

namespace FuncTraitsTests
{
	struct SingleZeroParamCallOperator
	{
		int operator()()
		{
			return 42;
		}
	};

	struct SingleZeroParamConstCallOperator
	{
		int operator()() const
		{
			return 42;
		}
	};

	struct SingleNonZeroParamCallOperator
	{
		int operator()(float)
		{
			return 42;
		}
	};

	static auto LambdaFunc = [](float) {return 42; };

	using MutableLambdaFunc = decltype([]() mutable {});

	static auto LambdaFuncWithCapture = [ret = 42](float) {return ret; };

	int PureFunction(float) { return 42; }

	struct StaticFunc
	{
		static int Foo(float)
		{
			return 42;
		}
	};

	using TemplatedLambdaWithParam = decltype([]<typename T>(float) { return 42; });
	using AutoLambdaWithParam = decltype([](auto, float) { return 42; });

	template<typename TestType, typename ExpectedRetType, typename ExpectedParamType, typename ExpectedCallSig, typename ExpectedObjType, bool ExpectedIsConst, typename... InExtraArgs>
	struct FuncSigTest
	{
		using Traits = TFuncTraits<TestType, InExtraArgs...>;
		static constexpr bool Passed =
			std::is_same_v<ExpectedRetType, Traits::ReturnType>&&
			std::is_same_v<ExpectedParamType, Traits::ParamTypes>&&
			std::is_same_v<ExpectedCallSig, Traits::CallSignature>&&
			std::is_same_v<ExpectedObjType, Traits::ObjType>&&
			ExpectedIsConst == Traits::IsConst;
	};

	static_assert(
		FuncSigTest<
		SingleZeroParamCallOperator,
		int,
		TypeList<>,
		int(),
		SingleZeroParamCallOperator,
		false>::Passed);

	static_assert(
		FuncSigTest<
		SingleNonZeroParamCallOperator,
		int,
		TypeList<float>,
		int(float),
		SingleNonZeroParamCallOperator,
		false>::Passed);

	static_assert(
		FuncSigTest<
		SingleZeroParamConstCallOperator,
		int,
		TypeList<>,
		int(),
		SingleZeroParamConstCallOperator,
		true>::Passed);

	static_assert(
		FuncSigTest<
		decltype(LambdaFunc),
		int,
		TypeList<float>,
		int(float),
		decltype(LambdaFunc),
		true>::Passed);

	static_assert(
		FuncSigTest<
		MutableLambdaFunc,
		void,
		TypeList<>,
		void(),
		MutableLambdaFunc,
		false>::Passed);

	static_assert(
		FuncSigTest<
		decltype(PureFunction),
		int,
		TypeList<float>,
		int(float),
		void,
		false>::Passed);

	static_assert(
		FuncSigTest<
		decltype(StaticFunc::Foo),
		int,
		TypeList<float>,
		int(float),
		void,
		false>::Passed);

	static_assert(
		FuncSigTest<
		TemplatedLambdaWithParam,
		int,
		TypeList<float>,
		int(float),
		TemplatedLambdaWithParam,
		true,
		TypeList<short>>::Passed);

	static_assert(
		FuncSigTest<
		AutoLambdaWithParam,
		int,
		TypeList<short, float>,
		int(short, float),
		AutoLambdaWithParam,
		true,
		TypeList<short>>::Passed);
}