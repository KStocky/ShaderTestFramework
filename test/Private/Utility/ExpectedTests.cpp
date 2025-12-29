

#include <Utility/Expected.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <sstream>

namespace ExpectedTests
{
    struct OStreamableType
    {
        inline static const std::string ExpectedString{ "Hello there" };

        friend std::ostream& operator<<(std::ostream& InOutStream, const OStreamableType&)
        {
            InOutStream << ExpectedString;
            return InOutStream;
        }
    };

    struct NotOStreamableType {};

    const std::string ExpectedUnstreamableString{ stf::TypeToString<NotOStreamableType>() };

    template<typename T, typename E>
    std::string StreamExpected(bool InValid)
    {
        const stf::Expected<T, E> expected = [&]() -> stf::Expected<T, E>
            {
                if (InValid)
                {
                    return stf::Expected<T, E>{T{}};
                }
                else
                {
                    return stf::Unexpected<E>{E{}};
                }
            }();

        std::stringstream buff;

        buff << expected;

        return buff.str();
    }
}

SCENARIO("Expected tests")
{
    using namespace stf;
    using namespace ExpectedTests;
    using Catch::Matchers::ContainsSubstring;

    const auto [given, actual, expected] = GENERATE(
        table<std::string, std::string, std::string>
        (
            {
                std::tuple{
                    "Valid expected has steamable value",
                    StreamExpected<OStreamableType, std::string>(true),
                    OStreamableType::ExpectedString
                },
                std::tuple{
                    "Invalid expected has unstreamable value",
                    StreamExpected<std::string, NotOStreamableType>(false),
                    ExpectedUnstreamableString
                },
            std::tuple{
                    "Invalid expected has steamable value",
                    StreamExpected<std::string, OStreamableType>(false),
                    OStreamableType::ExpectedString
                },
                std::tuple{
                    "Valid expected has unstreamable value",
                    StreamExpected<NotOStreamableType, std::string>(true),
                    ExpectedUnstreamableString
                }
            }
        )
    );


    GIVEN(given)
    {
        WHEN("streamed")
        {
            THEN("Expected string streamed")
            {
                REQUIRE_THAT(actual, ContainsSubstring(expected));
            }
        }
    }

}