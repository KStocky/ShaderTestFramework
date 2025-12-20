
#include <Utility/VersionedIndex.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace VersionedIndexCompileTests
{
    using namespace stf;
    template<typename T, u32 IndexBits>
    concept CValidInstantiation = requires()
    {
        { VersionedIndex<T, IndexBits>{} };
    };

    static_assert(!CValidInstantiation<u32, 32>);
    static_assert(!CValidInstantiation<u16, 31>);
    static_assert(CValidInstantiation<u32, 31>);
    static_assert(CValidInstantiation<u32, 24>);

    static_assert(VersionedIndex<u8, 4>::MaxIndex == (1 << 4) - 1);

    static_assert(VersionedIndex<u8, 5>::MaxVersion == (1 << 3) - 1);
}

SCENARIO("VersionedIndexTests - Valid Constructions")
{
    using namespace stf;
    static constexpr u32 IndexBits = 24;
    using u32Handle = VersionedIndex<u32, IndexBits>;

    const auto [given, handle, expectedIndex] = GENERATE(
        table<std::string, u32Handle, u32>
        (
            {
                std::tuple{"Default constructed", u32Handle{}, 0 },
                std::tuple{"Initialized with max index", u32Handle{u32Handle::MaxIndex}, u32Handle::MaxIndex }
            }
        )
    );

    GIVEN(given)
    {
        THEN("State is as expected")
        {
            REQUIRE(handle.GetIndex() == expectedIndex);
            REQUIRE(handle.GetVersion() == 0u);
            REQUIRE(handle == handle);
            REQUIRE_FALSE(handle != handle);

            AND_WHEN("versioned")
            {
                const auto versioned = handle.Next();

                THEN("versioned handle has same index but is not equal")
                {
                    REQUIRE(versioned.GetIndex() == expectedIndex);
                    REQUIRE(versioned.GetVersion() == 1u);
                    REQUIRE(handle != versioned);
                    REQUIRE_FALSE(handle == versioned);

                    AND_WHEN("and when versioned more than the max version times")
                    {
                        const auto onePlusMaxVersion = 
                            [&]()
                            {
                                auto ret = handle;
                                for (i32 i = 0; i <= u32Handle::MaxVersion; ++i)
                                {
                                    ret = ret.Next();
                                }

                                return ret;
                            }();

                        THEN("version wraps and is now equal to original handle")
                        {
                            REQUIRE(onePlusMaxVersion == handle);
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("VersionedIndexTests - Invalid Construction")
{
    using namespace stf;
    static constexpr u32 IndexBits = 24;
    using u32Handle = VersionedIndex<u32, IndexBits>;

    GIVEN("Constructed with index greater than max")
    {
        THEN("construction throws")
        {
            REQUIRE_THROWS(u32Handle{ u32Handle::MaxIndex + 1 });
        }
    }
}