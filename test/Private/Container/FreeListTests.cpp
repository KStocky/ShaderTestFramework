
#include "TestUtilities/ErrorMatchers.h"

#include <Platform.h>

#include <Container/FreeList.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("FreeListTests")
{
    using namespace stf;

    using FreeListType = FreeList<i32>;

    auto getResource =
        [](const FreeListType& InFreeList, const FreeListType::Handle InHandle)
        {
            const auto ret = InFreeList.Get(InHandle);
            REQUIRE(ret);
            return ret.value();
        };

    auto resourceGenerator =
        [id = 0]() mutable
        {
            return id++;
        };

    GIVEN("An empty free list")
    {
        FreeListType freeList;

        WHEN("Resource requested")
        {
            const auto firstHandle = freeList.Manage(resourceGenerator());

            REQUIRE(freeList.ValidateHandle(firstHandle));
            const auto firstResource = getResource(freeList, firstHandle);

            AND_WHEN("resource is released")
            {
                const auto releaseResult = freeList.Release(firstHandle);
                REQUIRE(releaseResult);

                THEN("handle is no longer valid")
                {
                    const auto getResult = freeList.Get(firstHandle);
                    REQUIRE_FALSE(getResult);

                    REQUIRE_THAT(getResult.error(), ErrorContainsFormat(Errors::FreeList::StaleHandle(0, 0).Format()));
                }

                AND_WHEN("resource is acquired again")
                {
                    const auto secondHandle = freeList.Manage(resourceGenerator());
                    REQUIRE(freeList.ValidateHandle(secondHandle));
                    const auto secondResource = getResource(freeList, secondHandle);

                    THEN("first and second resource are different")
                    {
                        REQUIRE(firstResource != secondResource);
                    }

                    THEN("trying to access first resource fails")
                    {
                        const auto getResultForFirst = freeList.Get(firstHandle);

                        REQUIRE_FALSE(getResultForFirst);
                        REQUIRE_THAT(getResultForFirst.error(), ErrorContainsFormat(Errors::FreeList::StaleHandle(0, 0).Format()));
                    }
                }
            }
        }
    }
}