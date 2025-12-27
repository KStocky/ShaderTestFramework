
#include <Utility/Concepts.h>
#include <Utility/Error.h>
#include <Utility/StringLiteral.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <format>
#include <string_view>
#include <utility>

namespace stf::ErrorFragmentCompileTests
{
    static_assert(Formattable<ErrorFragment, char>);
}

namespace stf::ErrorFragmentCompileTests::ConstructionTests
{
    struct FormattableType {};
    struct UnformattableType {};
}

template<>
struct std::formatter<stf::ErrorFragmentCompileTests::ConstructionTests::FormattableType> : std::formatter<string_view> {
    auto format(const auto&, auto& ctx) const {
        return std::formatter<string_view>::format("Hi", ctx);
    }
};

namespace stf::ErrorFragmentCompileTests::ConstructionTests
{
    template<typename... ArgTypes>
    concept TestConstructError = requires(ArgTypes&&... InArgs)
    {
        { ErrorFragment::Make<"Test">(std::forward<ArgTypes>(InArgs)...) };
    };

    static_assert(Formattable<FormattableType, char>);
    static_assert(!Formattable<UnformattableType, char>);

    static_assert(TestConstructError<FormattableType>, "Expected a single formattable argument to be valid");
    static_assert(TestConstructError<FormattableType, FormattableType, FormattableType, FormattableType>, "Expected many formattable arguments to be valid");
    static_assert(TestConstructError<>, "Expected zero arguments to be valid");

    static_assert(!TestConstructError<UnformattableType>, "Expected a single unformattable argument to be not be valid");
    static_assert(!TestConstructError<FormattableType, FormattableType, UnformattableType, FormattableType>, "Expected a single unformattable argument, among formattable arguments to be not be valid");
}

namespace stf::ErrorCompileTests
{
    static_assert(Formattable<Error, char>);
}

SCENARIO("ErrorFragment Tests")
{
    using namespace stf;
    using Catch::Matchers::ContainsSubstring;

    GIVEN("No args")
    {
        static constexpr FixedString expected = "Test";
        const auto noArgs = ErrorFragment::Make<expected>();

        THEN("State has expected")
        {
            REQUIRE(noArgs.Format().View() == expected.View());
            REQUIRE(noArgs.Error() == expected.View());
            REQUIRE(noArgs.HasFormat(StringLiteral{ expected.View() }));
        }
    }

    GIVEN("Two error fragments with same format and no args")
    {
        static constexpr FixedString expected = "Test";
        const auto errorFrag1 = ErrorFragment::Make<expected>();
        const auto errorFrag2 = ErrorFragment::Make<expected>();

        THEN("are equal")
        {
            REQUIRE(errorFrag1 == errorFrag2);
            REQUIRE_FALSE(errorFrag1 != errorFrag2);

            REQUIRE(errorFrag1.HasSameFormat(errorFrag2));
        }
    }

    GIVEN("Two error fragments with same format and same args")
    {
        static constexpr FixedString expected = "Test {}";
        constexpr i32 expectedArg = 42;
        const auto errorFrag1 = ErrorFragment::Make<expected>(expectedArg);
        const auto errorFrag2 = ErrorFragment::Make<expected>(expectedArg);

        THEN("are equal and has expected arg")
        {
            REQUIRE(errorFrag1 == errorFrag2);
            REQUIRE_FALSE(errorFrag1 != errorFrag2);

            REQUIRE(errorFrag1.HasSameFormat(errorFrag2));

            REQUIRE_THAT(std::string{ errorFrag1.Error() }, ContainsSubstring(std::format("{}", expectedArg)));
            REQUIRE_THAT(std::string{ errorFrag2.Error() }, ContainsSubstring(std::format("{}", expectedArg)));
        }
    }

    GIVEN("Two error fragments with same format and different args")
    {
        static constexpr FixedString expected = "Test {}";
        constexpr i32 expectedArg1 = 42;
        constexpr i32 expectedArg2 = 24;
        const auto errorFrag1 = ErrorFragment::Make<expected>(expectedArg1);
        const auto errorFrag2 = ErrorFragment::Make<expected>(expectedArg2);

        THEN("are not equal and have expected arg")
        {
            REQUIRE(errorFrag1 != errorFrag2);
            REQUIRE_FALSE(errorFrag1 == errorFrag2);

            REQUIRE(errorFrag1.HasSameFormat(errorFrag2));

            REQUIRE_THAT(std::string{ errorFrag1.Error() }, ContainsSubstring(std::format("{}", expectedArg1)));
            REQUIRE_THAT(std::string{ errorFrag2.Error() }, ContainsSubstring(std::format("{}", expectedArg2)));
        }
    }

    GIVEN("Two error fragments with different format and no args")
    {
        static constexpr FixedString expected1 = "Test";
        static constexpr FixedString expected2 = "Other Test";
        const auto errorFrag1 = ErrorFragment::Make<expected1>();
        const auto errorFrag2 = ErrorFragment::Make<expected2>();

        THEN("are not equal")
        {
            REQUIRE(errorFrag1 != errorFrag2);
            REQUIRE_FALSE(errorFrag1 == errorFrag2);

            REQUIRE_FALSE(errorFrag1.HasSameFormat(errorFrag2));
        }
    }

    GIVEN("Two error fragments with different format and same args")
    {
        static constexpr FixedString expected1 = "Test {}";
        static constexpr FixedString expected2 = "Other Test {}";
        constexpr i32 expectedArg = 42;
        const auto errorFrag1 = ErrorFragment::Make<expected1>(expectedArg);
        const auto errorFrag2 = ErrorFragment::Make<expected2>(expectedArg);

        THEN("are not equal and have expected arg")
        {
            REQUIRE(errorFrag1 != errorFrag2);
            REQUIRE_FALSE(errorFrag1 == errorFrag2);

            REQUIRE_FALSE(errorFrag1.HasSameFormat(errorFrag2));

            REQUIRE_THAT(std::string{ errorFrag1.Error() }, ContainsSubstring(std::format("{}", expectedArg)));
            REQUIRE_THAT(std::string{ errorFrag2.Error() }, ContainsSubstring(std::format("{}", expectedArg)));
        }
    }

    GIVEN("Two error fragments with different format and different args")
    {
        static constexpr FixedString expected1 = "Test {}";
        static constexpr FixedString expected2 = "Other Test {}";
        constexpr i32 expectedArg1 = 42;
        constexpr i32 expectedArg2 = 24;
        const auto errorFrag1 = ErrorFragment::Make<expected1>(expectedArg1);
        const auto errorFrag2 = ErrorFragment::Make<expected2>(expectedArg2);

        THEN("are not equal and have expected arg")
        {
            REQUIRE(errorFrag1 != errorFrag2);
            REQUIRE_FALSE(errorFrag1 == errorFrag2);

            REQUIRE_FALSE(errorFrag1.HasSameFormat(errorFrag2));

            REQUIRE_THAT(std::string{ errorFrag1.Error() }, ContainsSubstring(std::format("{}", expectedArg1)));
            REQUIRE_THAT(std::string{ errorFrag2.Error() }, ContainsSubstring(std::format("{}", expectedArg2)));
        }
    }
}

SCENARIO("Error Tests")
{
    using namespace stf;
    using Catch::Matchers::ContainsSubstring;

    GIVEN("Default constructed error")
    {
        Error error{};

        THEN("prints empty string")
        {
            REQUIRE(std::format("{}", error) == std::string{});
        }

        WHEN("Appended")
        {
            const auto errorFrag1 = ErrorFragment::Make<"Error 1">();

            error.Append(errorFrag1);

            THEN("Error contains error fragment")
            {
                REQUIRE(error.HasFragmentWithFormat(errorFrag1.Format()));

                REQUIRE_THAT(std::format("{}", error), ContainsSubstring(std::format("{}", errorFrag1.Error())));
            }

            AND_WHEN("Appended to again")
            {
                const auto errorFrag2 = ErrorFragment::Make<"Error 2: {}, {}">(42, 56);

                error += errorFrag2;

                THEN("Both error fragments exist and the latest fragment is before the first")
                {
                    REQUIRE(error.HasFragmentWithFormat(errorFrag1.Format()));
                    REQUIRE(error.HasFragmentWithFormat(errorFrag2.Format()));

                    const auto errorMessage = std::format("{}", error);
                    const auto error1Range = std::ranges::search(errorMessage, errorFrag1.Error());
                    const auto error2Range = std::ranges::search(errorMessage, errorFrag2.Error());

                    REQUIRE(error1Range);
                    REQUIRE(error2Range);

                    REQUIRE(error2Range.cend() < error1Range.cbegin());
                }
            }
        }
    }
}