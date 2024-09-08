
#include <D3D12/DescriptorFreeListAllocator.h>

#include <ranges>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("DescriptorFreeListAllocatorTests")
{
    using namespace stf;
	SECTION("Empty allocator tests")
	{
		auto [given, allocator] = GENERATE(
			table<std::string, DescriptorFreeListAllocator>
			(
				{
					std::tuple{"Default constructed", DescriptorFreeListAllocator{} }, 
					std::tuple{"Empty Range", DescriptorFreeListAllocator{DescriptorRange{}} }
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
					REQUIRE(allocation.error() == DescriptorFreeListAllocator::EErrorType::EmptyError);
				}
			}
		}
	}

	GIVEN("larger capacity than size")
	{
		static constexpr u32 capacity = 2;
		static constexpr u32 increment = 2;
		static constexpr u32 numAllocationsBefore = capacity - 2;
		static constexpr u32 expectedSize = numAllocationsBefore + 1;
		const auto allocatorRange = DescriptorRange({ {}, {} }, capacity, increment);
		DescriptorFreeListAllocator allocator{ allocatorRange };

		WHEN("Allocate")
		{
			const auto handle = allocator.Allocate();

			THEN("allocation succeeds")
			{
				REQUIRE(capacity == allocator.GetCapacity());
				REQUIRE(expectedSize == allocator.GetSize());
			}
		}
	}

	GIVEN("size is equal to capacity")
	{
		static constexpr u32 capacity = 2;
		static constexpr u32 increment = 2;
		static constexpr u32 numAllocationsBefore = capacity - 1;
		static constexpr u32 expectedSize = numAllocationsBefore + 1;
		const auto allocatorRange = DescriptorRange({ {}, {} }, capacity, increment);
		DescriptorFreeListAllocator allocator{ allocatorRange };

		for ([[maybe_unused]] const auto allocations : std::ranges::iota_view(0u, numAllocationsBefore))
		{
			const auto handle = allocator.Allocate();
		}

		WHEN("Allocate")
		{
			const auto handle = allocator.Allocate();

			THEN("allocation succeeds")
			{
				REQUIRE(capacity == allocator.GetCapacity());
				REQUIRE(expectedSize == allocator.GetSize());
			}
		}
	}

	GIVEN("size is less than capacity")
	{
		static constexpr u32 capacity = 2;
		static constexpr u32 increment = 2;
		static constexpr u32 numAllocationsBefore = capacity;
		static constexpr u32 expectedSize = numAllocationsBefore;
		const auto allocatorRange = DescriptorRange({ {}, {} }, capacity, increment);
		DescriptorFreeListAllocator allocator{ allocatorRange };

		for ([[maybe_unused]] const auto allocations : std::ranges::iota_view(0u, numAllocationsBefore))
		{
			const auto handle = allocator.Allocate();
		}

		WHEN("Allocate")
		{
			const auto handle = allocator.Allocate();

			THEN("allocation fails")
			{
				REQUIRE(!handle);
				REQUIRE(handle.error() == DescriptorFreeListAllocator::EErrorType::EmptyError);
				REQUIRE(capacity == allocator.GetCapacity());
				REQUIRE(expectedSize == allocator.GetSize());
			}
		}
	}

	GIVEN("descriptor has not been allocated")
	{
		static constexpr u32 capacity = 2;
		static constexpr u32 increment = 2;
		static constexpr u32 expectedSize = 0;
		const auto allocatorRange = DescriptorRange({ {}, {} }, capacity, increment);
		DescriptorFreeListAllocator allocator{ allocatorRange };

		WHEN("descriptor is released")
		{
			const auto handle = allocatorRange.First();
			const auto result = allocator.Release(*handle);

			THEN("Release fails")
			{
				REQUIRE(!result);
				REQUIRE(result.error() == DescriptorFreeListAllocator::EErrorType::InvalidDescriptor);
				REQUIRE(capacity == allocator.GetCapacity());
				REQUIRE(expectedSize == allocator.GetSize());
			}
		}
	}

	GIVEN("Descriptor is not in range")
	{
		static constexpr u32 capacity = 2;
		static constexpr u32 increment = 2;
		static constexpr u32 expectedSize = 0;
		const auto allocatorRange = DescriptorRange({ {}, {} }, capacity, increment);
		const auto allocatorRange2 = DescriptorRange({ {capacity}, {capacity} }, capacity, increment);
		DescriptorFreeListAllocator allocator{ allocatorRange };
		WHEN("Descriptor is released")
		{
			const auto handle = allocatorRange2.First();
			const auto result = allocator.Release(*handle);
			THEN("Release fails")
			{
				REQUIRE(!result);
				REQUIRE(result.error() == DescriptorFreeListAllocator::EErrorType::InvalidDescriptor);
				REQUIRE(capacity == allocator.GetCapacity());
				REQUIRE(expectedSize == allocator.GetSize());
			}
		}
	}

	GIVEN("Descriptor has been allocated")
	{
		static constexpr u32 capacity = 2;
		static constexpr u32 increment = 2;
		static constexpr u32 expectedSize = 0;
		const auto allocatorRange = DescriptorRange({ {}, {} }, capacity, increment);
		DescriptorFreeListAllocator allocator{ allocatorRange };

		const auto handle = allocator.Allocate();
		WHEN("Descriptor is released")
		{
			const auto result = allocator.Release(*handle);
			THEN("Release succeeds")
			{
				REQUIRE(result);
				REQUIRE(capacity == allocator.GetCapacity());
				REQUIRE(expectedSize == allocator.GetSize());
			}

			WHEN("Descriptor is released again")
			{
				const auto second = allocator.Release(*handle);
				THEN("Release fails")
				{
					REQUIRE(!second);
					REQUIRE(second.error() == DescriptorFreeListAllocator::EErrorType::InvalidDescriptor);
					REQUIRE(capacity == allocator.GetCapacity());
					REQUIRE(expectedSize == allocator.GetSize());
				}
			}
		}
	}
}