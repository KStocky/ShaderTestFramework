
#include <D3D12/BindlessFreeListAllocator.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("BindlessFreeListAllocatorTests")
{
    using namespace stf;
    SECTION("Empty allocator tests")
    {
        auto [given, allocator] = GENERATE(
            table<std::string, BindlessFreeListAllocator>
            (
                {
                    std::tuple{"Default constructed", BindlessFreeListAllocator{} },
                    std::tuple{"Empty Range", BindlessFreeListAllocator{{.NumDescriptors = 0}} }
                }
            )
        );

        GIVEN(given)
        {
            REQUIRE(0u == allocator.GetSize());
            REQUIRE(0u == allocator.GetCapacity());

            WHEN("Allocate")
            {
                const auto allocation = allocator.Allocate();
                THEN("return expected error")
                {
                    REQUIRE(!allocation);
                    REQUIRE(allocation.error() == BindlessFreeListAllocator::EErrorType::EmptyError);
                }
            }
        }
    }

    GIVEN("Non Empty allocator tests")
    {
        static constexpr u32 initialCapacity = 5;
        BindlessFreeListAllocator allocator{ {.NumDescriptors = initialCapacity} };

        THEN("Initial state is as expected")
        {
            REQUIRE(initialCapacity == allocator.GetCapacity());
            REQUIRE(0 == allocator.GetSize());
        }

        WHEN("attempt to shrink allocator")
        {
            const auto result = allocator.Resize(initialCapacity - 1);

            THEN("shrink fails")
            {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(initialCapacity == allocator.GetCapacity());
                REQUIRE(0 == allocator.GetSize());
            }
        }

        AND_GIVEN("a second non empty allocator")
        {
            static constexpr u32 otherCapacity = initialCapacity + 1;
            BindlessFreeListAllocator otherAllocator{ {.NumDescriptors = otherCapacity } };

            WHEN("allocation made on second allocator")
            {
                const auto invalidAllocation = otherAllocator.Allocate();

                THEN("allocation succeeded on other allocator")
                {
                    REQUIRE(invalidAllocation.has_value());
                }

                WHEN("release attempted using initial allocator")
                {
                    const auto releaseResult = allocator.Release(invalidAllocation.value());

                    THEN("release failed")
                    {
                        REQUIRE_FALSE(releaseResult.has_value());
                        REQUIRE(releaseResult.error() == BindlessFreeListAllocator::EErrorType::IndexAlreadyReleased);
                    }
                }
            }

            WHEN("final allocation available from second allocation acquired")
            {
                for (u32 i = 0; i < otherCapacity - 1; ++i)
                {
                    const auto allocation = otherAllocator.Allocate();
                    REQUIRE(allocation.has_value());
                }

                const auto finalAllocation = otherAllocator.Allocate();

                THEN("allocation is valid and allocator is full")
                {
                    REQUIRE(finalAllocation.has_value());
                    REQUIRE(otherCapacity == otherAllocator.GetCapacity());
                    REQUIRE(otherCapacity == otherAllocator.GetSize());
                }

                AND_WHEN("final allocation freed from initial allocator")
                {
                    const auto finalReleaseOnInitialAllocatorResult = allocator.Release(finalAllocation.value());

                    THEN("release failed")
                    {
                        REQUIRE_FALSE(finalReleaseOnInitialAllocatorResult.has_value());
                        REQUIRE(finalReleaseOnInitialAllocatorResult.error() == BindlessFreeListAllocator::EErrorType::InvalidIndex);
                    }
                }

                AND_WHEN("final allocation freed from other allocator")
                {
                    const auto finalReleaseOnInitialAllocatorResult = otherAllocator.Release(finalAllocation.value());

                    THEN("release succeeded")
                    {
                        REQUIRE(finalReleaseOnInitialAllocatorResult.has_value());
                    }
                }
            }
        }

        WHEN("Single allocation made")
        {
            const auto bindlessIndex1 = allocator.Allocate();

            THEN("State is as expected")
            {
                REQUIRE(bindlessIndex1.has_value());
                REQUIRE(bindlessIndex1.value() == 0);
                REQUIRE(initialCapacity == allocator.GetCapacity());
                REQUIRE(1 == allocator.GetSize());
            }

            AND_WHEN("allocation is released")
            {
                const auto result = allocator.Release(bindlessIndex1.value());

                THEN("State is as expected")
                {
                    REQUIRE(result.has_value());
                    REQUIRE(initialCapacity == allocator.GetCapacity());
                    REQUIRE(0 == allocator.GetSize());
                }

                AND_WHEN("the same allocation released twice")
                {
                    const auto secondReleaseResult = allocator.Release(bindlessIndex1.value());

                    THEN("Release fails")
                    {
                        REQUIRE_FALSE(secondReleaseResult.has_value());
                        REQUIRE(secondReleaseResult.error() == BindlessFreeListAllocator::EErrorType::IndexAlreadyReleased);
                        REQUIRE(initialCapacity == allocator.GetCapacity());
                        REQUIRE(0 == allocator.GetSize());
                    }
                }
            }
        }

        WHEN("Allocator is full")
        {
            using AllocationType = decltype(allocator.Allocate());
            std::vector<AllocationType> allocations;
            for (u32 i = 0; i < initialCapacity; ++i)
            {
                allocations.push_back(allocator.Allocate());
            }

            THEN("State is as expected")
            {
                for (const auto allocation : allocations)
                {
                    REQUIRE(allocation.has_value());
                }

                REQUIRE(initialCapacity == allocator.GetCapacity());
                REQUIRE(initialCapacity == allocator.GetSize());
            }

            THEN("All allocations are unique")
            {
                for (u64 i = 0; i < allocations.size() - 1; ++i)
                {
                    for (u64 j = i + 1; j < allocations.size(); ++j)
                    {
                        REQUIRE(allocations[i].value() != allocations[j].value());
                    }
                }
            }

            AND_WHEN("allocation attempted")
            {
                const auto allocation = allocator.Allocate();

                THEN("allocation failed")
                {
                    REQUIRE_FALSE(allocation.has_value());
                    REQUIRE(initialCapacity == allocator.GetCapacity());
                    REQUIRE(initialCapacity == allocator.GetSize());
                }
            }

            AND_WHEN("second allocation is freed")
            {
                const auto release = allocator.Release(allocations[1].value());

                THEN("release succeeds")
                {
                    REQUIRE(release.has_value());
                    REQUIRE(initialCapacity == allocator.GetCapacity());
                    REQUIRE((initialCapacity - 1) == allocator.GetSize());
                }

                AND_WHEN("allocation made")
                {
                    const auto allocation = allocator.Allocate();

                    THEN("allocated index is the same as previously released")
                    {
                        REQUIRE(allocation.has_value());
                        REQUIRE(allocation.value() == allocations[1].value());
                    }
                }
            }

            AND_WHEN("all allocations are freed")
            {
                using ReleaseType = decltype(allocator.Release(std::declval<AllocationType>().value()));

                std::vector<ReleaseType> releases;
                for (const auto allocation : allocations)
                {
                    releases.push_back(allocator.Release(allocation.value()));
                }

                THEN("releases succeeded")
                {
                    for (const auto release : releases)
                    {
                        REQUIRE(release.has_value());
                    }
                    REQUIRE(initialCapacity == allocator.GetCapacity());
                    REQUIRE(0 == allocator.GetSize());
                }
            }

            AND_WHEN("allocator is doubled in size")
            {
                static constexpr u32 newCapacity = initialCapacity * 2;
                const auto resizeResult = allocator.Resize(newCapacity);

                THEN("resize succeeds")
                {
                    REQUIRE(resizeResult.has_value());
                    REQUIRE(newCapacity == allocator.GetCapacity());
                    REQUIRE(initialCapacity == allocator.GetSize());
                }

                AND_WHEN("allocation made")
                {
                    const auto allocation = allocator.Allocate();

                    THEN("allocation succeeds")
                    {
                        REQUIRE(allocation.has_value());
                        REQUIRE(newCapacity == allocator.GetCapacity());
                        REQUIRE(initialCapacity + 1 == allocator.GetSize());
                    }

                    THEN("allocation is unique")
                    {
                        for (const auto oldAllocation : allocations)
                        {
                            REQUIRE(allocation != oldAllocation);
                        }
                    }
                }
            }
        }
    }
}