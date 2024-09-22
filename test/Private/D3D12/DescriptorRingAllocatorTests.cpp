
#include <D3D12/DescriptorRingAllocator.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("DescriptorRingAllocatorTests")
{
    using namespace stf;
	SECTION("Empty allocator tests")
	{
		auto [given, allocator] = GENERATE(
			table<std::string, DescriptorRingAllocator>
			(
				{
					std::tuple{"Default constructed", DescriptorRingAllocator{} },
					std::tuple{"Empty Range", DescriptorRingAllocator{DescriptorRange{}} }
				}
			)
		);

		GIVEN(given)
		{
			REQUIRE(0u == allocator.GetSize());
			REQUIRE(0u == allocator.GetCapacity());

			WHEN("Allocate")
			{
				const auto allocation = allocator.Allocate(1);
				THEN("return expected error")
				{
					REQUIRE(!allocation);
					REQUIRE(allocation.error() == DescriptorRingAllocator::EErrorType::AllocationError);
				}
			}
		}
	}

	GIVEN("Allocator with non zero capacity")
	{
		static constexpr u32 capacity = 25;
		static constexpr u32 increment = 10;
		const DescriptorRange allocatorRange{ {}, capacity, increment };
		DescriptorRingAllocator allocator{ allocatorRange };

		REQUIRE(allocator.GetCapacity() == capacity);

		SECTION("Successful Allocation Tests")
		{
			auto [when, allocationSize] = GENERATE(
				table<std::string, u32>
				(
					{
						std::tuple{ "Allocation is smaller than capacity", capacity - 1 },
						std::tuple{ "Allocation is the same size as capacity", capacity }
					}
				)
			);

			WHEN(when)
			{
				THEN("reports that allocation will succeed")
				{
					REQUIRE(allocator.CanAllocate(allocationSize));
				}

				const auto allocatedRange = allocator.Allocate(allocationSize);

				THEN("Allocation succeeds")
				{
					REQUIRE(allocatedRange);
					REQUIRE(allocatedRange->GetSize() == allocationSize);
					REQUIRE(allocator.GetSize() == allocationSize);
				}
			}
		}

		WHEN("allocation is larger than capacity")
		{
			static constexpr u32 allocationSize = capacity + 1;

			THEN("reports that allocation will fail")
			{
				REQUIRE(!allocator.CanAllocate(allocationSize));
			}

			const auto allocatedRange = allocator.Allocate(allocationSize);

			THEN("Allocation fails")
			{
				REQUIRE(!allocatedRange);
				REQUIRE(allocatedRange.error() == DescriptorRingAllocator::EErrorType::AllocationError);
				REQUIRE(allocator.GetSize() == 0);
			}
		}

		AND_GIVEN("Allocation of half the capacity already made")
		{
			static constexpr uint32_t allocationSize = capacity / 2;
			const auto allocatedRange = allocator.Allocate(allocationSize);

			AND_GIVEN("release is not larger than allocation")
			{
				auto [when, releaseSize, expectedSize] = GENERATE(
					table<std::string, u32, u32>
					(
						{
							std::tuple{"release fewer descriptors than allocated", allocationSize - 1, 1},
							std::tuple{"relase same number of descriptors as allocated", allocationSize, 0}
						}
					)
				);

				WHEN(when)
				{
					const auto result = allocator.Release(releaseSize);

					THEN("Release succeeded")
					{
						REQUIRE(result);
						REQUIRE(allocator.GetCapacity() == capacity);
						REQUIRE(allocator.GetSize() == expectedSize);
					}
				}
			}

			WHEN("release is larger than allocation")
			{
				static constexpr u32 releaseSize = allocationSize + 1;
				static constexpr u32 expectedSize = allocationSize;

				const auto result = allocator.Release(releaseSize);

				THEN("release failed")
				{
					REQUIRE(!result);
					REQUIRE(allocator.GetCapacity() == capacity);
					REQUIRE(allocator.GetSize() == expectedSize);
				}
			}
		}

		AND_GIVEN("allocation of 1 less than capacity has been made and freed")
		{
			static constexpr u32 initialAllocationSize = capacity - 1;
			static constexpr u32 releaseSize = initialAllocationSize;

			const auto allocation = allocator.Allocate(initialAllocationSize);
			const auto releaseResult = allocator.Release(releaseSize);

			REQUIRE(releaseResult);

			AND_GIVEN("allocation of half the size")
			{
				static constexpr u32 allocationSize = capacity / 2;
				static constexpr u32 expectedSize = (capacity - initialAllocationSize) + allocationSize;
				

				WHEN("queried")
				{
					const auto result = allocator.CanAllocate(allocationSize);
					THEN("succeeds")
					{
						REQUIRE(result);
					}
				}

				WHEN("allocation made")
				{
					const auto result = allocator.Allocate(allocationSize);

					THEN("allocation succeeds")
					{
						REQUIRE(result);
						REQUIRE(allocator.GetCapacity() == capacity);
						REQUIRE(allocator.GetSize() == expectedSize);
					}
				}
			}
		}

		AND_GIVEN("allocation has been made and fewer descriptors have been freed")
		{
			static constexpr u32 releaseSize = 1;
			static constexpr u32 initialAllocationSize = capacity - 5 + releaseSize;

			const auto allocation = allocator.Allocate(initialAllocationSize);
			const auto releaseResult = allocator.Release(releaseSize);

			REQUIRE(releaseResult);

			AND_GIVEN("allocation that would loop more than capacity")
			{
				static constexpr u32 allocationSize = 5;

				WHEN("queried")
				{
					const auto result = allocator.CanAllocate(allocationSize);
					THEN("fails")
					{
						REQUIRE(!result);
					}
				}

				WHEN("allocation made")
				{
					const auto result = allocator.Allocate(allocationSize);

					THEN("allocation fails")
					{
						static constexpr u32 expectedSize = initialAllocationSize - releaseSize;
						REQUIRE(!result);
						REQUIRE(result.error() == DescriptorRingAllocator::EErrorType::AllocationError);
						REQUIRE(allocator.GetCapacity() == capacity);
						REQUIRE(allocator.GetSize() == expectedSize);
					}
				}
			}
		}
	}
}