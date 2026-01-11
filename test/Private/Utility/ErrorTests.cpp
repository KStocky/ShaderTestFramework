
#include "TestUtilities/ErrorMatchers.h"
#include <Utility/Concepts.h>
#include <Utility/Error.h>
#include <Utility/StringLiteral.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <format>
#include <sstream>
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
                REQUIRE_THAT(error, ErrorContainsFormat(errorFrag1.Format()));
                REQUIRE_THAT(error, ErrorContains(errorFrag1));

                const auto formattedError = std::format("{}", error);
                const auto streamError = [&]()
                    {
                        std::stringstream stringBuffer;
                        stringBuffer << error;
                        return stringBuffer.str();
                    }();

                REQUIRE(formattedError == streamError);

                REQUIRE_THAT(formattedError, ContainsSubstring(std::format("{}", errorFrag1.Error())));
            }

            AND_WHEN("Appended to again")
            {
                const auto errorFrag2 = ErrorFragment::Make<"Error 2: {}, {}">(42, 56);

                error += errorFrag2;

                THEN("Both error fragments exist and the latest fragment is before the first")
                {
                    REQUIRE_THAT(error, ErrorContainsFormat(errorFrag1.Format()));
                    REQUIRE_THAT(error, ErrorContainsFormat(errorFrag2.Format()));

                    const auto errorMessage = std::format("{}", error);

                    const auto streamError = [&]()
                        {
                            std::stringstream stringBuffer;
                            stringBuffer << error;
                            return stringBuffer.str();
                        }();

                    REQUIRE(errorMessage == streamError);

                    const auto error1Range = std::ranges::search(errorMessage, errorFrag1.Error());
                    const auto error2Range = std::ranges::search(errorMessage, errorFrag2.Error());

                    REQUIRE(error1Range);
                    REQUIRE(error2Range);

                    REQUIRE(error2Range.cend() < error1Range.cbegin());
                }
            }
        }
    }

    GIVEN("Constructed from a fragment")
    {
        static constexpr FixedString expectedFormat{ "Error 1" };
        const auto error = Error::FromFragment<expectedFormat>();

        THEN("contains expected fragment")
        {
            REQUIRE_THAT(error, ErrorContainsFormat(expectedFormat.Literal()));
        }
    }

    GIVEN("Two errors constructed by appending different fragments")
    {

        const ErrorFragment frag1 = ErrorFragment::Make<"1">();
        const ErrorFragment frag2 = ErrorFragment::Make<"2">();
        const ErrorFragment frag3 = ErrorFragment::Make<"3">();
        const ErrorFragment frag4 = ErrorFragment::Make<"4">();

        const Error error1 = frag1 + frag2;
        const Error error2 = frag3 + frag4;

        THEN("Errors are as expected")
        {
            REQUIRE(error1 != error2);
            REQUIRE(error1.HasFragmentWithFormat(frag1.Format()));
            REQUIRE(error1.HasFragmentWithFormat(frag2.Format()));
            REQUIRE(error2.HasFragmentWithFormat(frag3.Format()));
            REQUIRE(error2.HasFragmentWithFormat(frag4.Format()));
            REQUIRE_FALSE(error2.HasFragmentWithFormat(frag1.Format()));
            REQUIRE_FALSE(error2.HasFragmentWithFormat(frag2.Format()));
            REQUIRE_FALSE(error1.HasFragmentWithFormat(frag3.Format()));
            REQUIRE_FALSE(error1.HasFragmentWithFormat(frag4.Format()));

            REQUIRE_THAT(error1, ErrorContainsFormat(frag1.Format()));
            REQUIRE_THAT(error1, ErrorContainsFormat(frag2.Format()));
            REQUIRE_THAT(error2, ErrorContainsFormat(frag3.Format()));
            REQUIRE_THAT(error2, ErrorContainsFormat(frag4.Format()));
            REQUIRE_THAT(error2, !ErrorContainsFormat(frag1.Format()));
            REQUIRE_THAT(error2, !ErrorContainsFormat(frag2.Format()));
            REQUIRE_THAT(error1, !ErrorContainsFormat(frag3.Format()));
            REQUIRE_THAT(error1, !ErrorContainsFormat(frag4.Format()));

            WHEN("errors are appended")
            {
                const Error error3 = error1 + error2;

                THEN("new error contains all fragments")
                {
                    REQUIRE(error3.HasFragmentWithFormat(frag1.Format()));
                    REQUIRE(error3.HasFragmentWithFormat(frag2.Format()));
                    REQUIRE(error3.HasFragmentWithFormat(frag3.Format()));
                    REQUIRE(error3.HasFragmentWithFormat(frag4.Format()));

                    REQUIRE_THAT(error3, ErrorContainsFormat(frag1.Format()));
                    REQUIRE_THAT(error3, ErrorContainsFormat(frag2.Format()));
                    REQUIRE_THAT(error3, ErrorContainsFormat(frag3.Format()));
                    REQUIRE_THAT(error3, ErrorContainsFormat(frag4.Format()));
                }
            }
        }
    }
}

SCENARIO("Error comparison tests")
{
    using namespace stf;
    const auto [given, left, right, expected] = GENERATE(
        table<std::string, Error, Error, bool>
        (
            {
                std::tuple
                {
                    "Errors with same format and no args",
                    Error::FromFragment<"Error">(),
                    Error::FromFragment<"Error">(),
                    true
                },
                std::tuple
                {
                    "Errors with different formats and no args",
                    Error::FromFragment<"Error">(),
                    Error::FromFragment<"OtherError">(),
                    false
                },
                std::tuple
                {
                    "Errors with same format and args",
                    Error::FromFragment<"Error {}">(42),
                    Error::FromFragment<"Error {}">(42),
                    true
                },
                std::tuple
                {
                    "Errors with same format and different args",
                    Error::FromFragment<"Error {}">(42),
                    Error::FromFragment<"Error {}">(24),
                    false
                },
                std::tuple
                {
                    "Errors with different format and same args",
                    Error::FromFragment<"Error 1{}">(42),
                    Error::FromFragment<"Error 2{}">(42),
                    false
                },
                std::tuple
                {
                    "Errors with different format and different args",
                    Error::FromFragment<"Error 1{}">(42),
                    Error::FromFragment<"Error 2{}">(24),
                    false
                },
                std::tuple
                {
                    "Errors with differing number of fragments",
                    Error::FromFragment<"Error 1">(),
                    Error::FromFragment<"Error 1">() += ErrorFragment::Make<"Error 2">(),
                    false
                }
            }
        )
    );

    GIVEN(given)
    {
        WHEN("compared")
        {
            const auto equalResult = left == right;
            const auto notEqualResult = left != right;

            if (expected)
            {
                THEN("compares as equal")
                {
                    REQUIRE(equalResult);
                    REQUIRE_FALSE(notEqualResult);
                }
            }
            else
            {
                THEN("compares as not equal")
                {
                    REQUIRE_FALSE(equalResult);
                    REQUIRE(notEqualResult);
                }
            }
        }
    }
}