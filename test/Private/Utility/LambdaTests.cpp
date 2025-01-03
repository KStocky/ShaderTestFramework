#include <Utility/Lambda.h>

namespace LambdaTypeTests
{
    using namespace stf;
	void SizeTests()
	{
		Lambda CapturelessLambda([]() {});
		Lambda LambdaWithConstCaptures([](const int&, const float&) {}, 5, 2.0f);

		static_assert(sizeof(CapturelessLambda) == 1);
		static_assert(sizeof(LambdaWithConstCaptures) == 8);

		Lambda BadlyPacked([](const double&, const bool&, const double&, const bool&) {}, 42.0, false, 21.0, true);
		Lambda WellPacked([](const double&, const double&, const bool&, const bool&) {}, 42.0, 21.0, false, true);

		static_assert(sizeof(BadlyPacked) == 24);
		static_assert(sizeof(WellPacked) == 24);

		using BadLanguage = decltype([d1 = 21.0, b1 = false, d2 = 42.0, b2 = true]() {});
		using GoodLanguage = decltype([d1 = 21.0, d2 = 42.0, b1 = false, b2 = true]() {});

		static_assert(sizeof(BadLanguage) == 32);
		static_assert(sizeof(GoodLanguage) == 24);

		static_assert(sizeof(BadlyPacked) < sizeof(BadLanguage));

		using ShortLanguage = decltype([s = short{ 42 }]() {});
		static_assert(sizeof(ShortLanguage) == 2);
	}
	

	void ConstCallTests()
	{
		Lambda ZeroCaptures([]() {});
		Lambda MutableZeroCaptures([]() mutable {});
		Lambda CaptureByValPassByConstRef([](const int&) {}, 5);

		int val = 42;
		Lambda CaptureByRefPassByConstRef([](const int&) {}, &val);

		static_assert(decltype(ZeroCaptures)::IsConstCall);
		static_assert(decltype(CaptureByValPassByConstRef)::IsConstCall);
		static_assert(decltype(CaptureByRefPassByConstRef)::IsConstCall);
		static_assert(!decltype(MutableZeroCaptures)::IsConstCall);
	}

	void CaptureByRefTests()
	{
		int val = 42;
		double otherVal = 2.0;

		Lambda CaptureByNonAutoRef([]([[maybe_unused]] int& InVal, [[maybe_unused]]double& InOtherVal) { return TypeList<decltype(InVal), decltype(InOtherVal)>{}; }, &val, &otherVal);
		using RetType = decltype(CaptureByNonAutoRef());

		static_assert(std::is_same_v<RetType::template Type<0>, int&>);
	}

	void PackingCaptureCallTests()
	{
		static constexpr Lambda CaptureReorder([](const double& InFirstDouble, const bool& InFirstBool, const double& InSecondDouble, const bool& InSecondBool) { return Tuple{InFirstDouble, InFirstBool, InSecondDouble, InSecondBool}; }, 4.0, false, 2.0, true);

		static constexpr auto Ret = CaptureReorder();

		static_assert(get<0>(Ret) == 4.0);
		static_assert(get<1>(Ret) == false);
		static_assert(get<2>(Ret) == 2.0);
		static_assert(get<3>(Ret) == true);
	}

	void CaptureTests()
	{
		using WithShortCapture = decltype(Lambda([](const short&) {}, short{ 42 }));

		static_assert(std::is_same_v<typename WithShortCapture::CaptureTypes::template Type<0>, const short>);

		using ModifyingCaptureByValue = decltype(Lambda([](auto& In) mutable { ++In; return In; }, 0));
	}

	template<auto InFunc, typename... InCaptureTypes>
	struct StaticAssertConditions
	{
		using Type = typename Lambda<InFunc, TypeList<InCaptureTypes...>, std::index_sequence_for<InCaptureTypes...>>::StaticAssertConditions;
	};

	template<auto InFunc, typename... InCaptureTypes>
	concept LambdaConstructionTest =
		StaticAssertConditions<InFunc, InCaptureTypes...>::Type::CaptureVarsArePassedByRef &&
		StaticAssertConditions<InFunc, InCaptureTypes...>::Type::CaptureByValVarsArePassedByConstRefForConstCall;

	void IllegalConstructionTests()
	{
		static_assert(LambdaConstructionTest < [](int&) mutable {}, int > );
		static_assert(LambdaConstructionTest < [](const int&) mutable {}, int > );
		static_assert(!LambdaConstructionTest < [](int&) {}, int > );
		static_assert(LambdaConstructionTest < [](const int&) {}, int > );

		static_assert(LambdaConstructionTest < [](int&) mutable {}, int* > );
		static_assert(LambdaConstructionTest < [](const int&) mutable {}, int* > );
		static_assert(LambdaConstructionTest < [](int&) {}, int* > );
		static_assert(LambdaConstructionTest < [](const int&) {}, int* > );

		static_assert(LambdaConstructionTest < [](auto&) mutable {}, int > );
		static_assert(LambdaConstructionTest < [](const auto&) mutable {}, int > );
		static_assert(LambdaConstructionTest < [](auto&) {}, int > );
		static_assert(LambdaConstructionTest < [](const auto&) {}, int > );

		static_assert(LambdaConstructionTest < [](auto&) mutable {}, int* > );
		static_assert(LambdaConstructionTest < [](const auto&) mutable {}, int* > );
		static_assert(LambdaConstructionTest < [](auto&) {}, int* > );
		static_assert(LambdaConstructionTest < [](const auto&) {}, int* > );

		static_assert(!LambdaConstructionTest < [](int) mutable {}, int > );
		static_assert(!LambdaConstructionTest < [](int) {}, int > );

		static_assert(!LambdaConstructionTest < [](int) mutable {}, int* > );
		static_assert(!LambdaConstructionTest < [](int) {}, int* > );
	}

	template<auto InFunc, typename InCaptureTypes, typename InCaptureIndices>
	struct FakeLambda;

	template<auto InFunc, typename... InCaptureTypes, size_t... InCaptureIndices>
	struct FakeLambda<InFunc, TypeList<InCaptureTypes...>, std::index_sequence<InCaptureIndices...>>
	{};

	void ConceptTests()
	{
		using CaptureLess = decltype(Lambda([]() {}));
		using Captures = decltype(Lambda([](auto&) {}, 5));
		using NotLambdaNonTemplate = int;
		using NotLambdaTemplate = FakeLambda < []() {}, TypeList<>, std::index_sequence<> > ;

		static_assert(LambdaType<CaptureLess>);
		static_assert(LambdaType<Captures>);
		static_assert(!LambdaType<NotLambdaNonTemplate>);
		static_assert(!LambdaType<NotLambdaTemplate>);
	}
}