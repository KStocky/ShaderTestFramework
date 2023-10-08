
#include <Platform.h>
#include <D3D12/Descriptor.h>

#include <functional>
#include <ranges>
#include <tuple>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("DescriptorRangeTests")
{
	SECTION("Invalid Range Tests")
	{
		const auto ErrorOr =
			[](auto InExpected)
			{
				if (InExpected)
				{
					return DescriptorRange::EErrorType::Success;
				}
				return InExpected.error();
			};

		auto [given, range, expectedSize] = GENERATE(
			table<std::string, DescriptorRange, u32>
			(
				{
					std::tuple{ "Empty range", DescriptorRange{}, 0 },
					std::tuple{ "Non empty with zero increment", DescriptorRange{ {}, 42, 0 }, 42 },
				}
			)
		);

		auto [when, errorOp, expectedError] = GENERATE_COPY(
			table<std::string, std::function<DescriptorRange::EErrorType(const DescriptorRange)>, DescriptorRange::EErrorType>
			(
				{
					std::tuple
					{ 
						"Get Index is called with empty handle", 
						[ErrorOr](const DescriptorRange InRange){ return ErrorOr(InRange.GetIndex(DescriptorHandle{})); },
						DescriptorRange::EErrorType::InvalidRange
					},
					std::tuple
					{ 
						"SubRange is called", 
						[ErrorOr](const DescriptorRange InRange){ return ErrorOr(InRange.Subrange(0, 0)); },
						DescriptorRange::EErrorType::InvalidRange
					},
					std::tuple
					{ 
						"First is called", 
						[ErrorOr](const DescriptorRange InRange){ return ErrorOr(InRange.First()); },
						DescriptorRange::EErrorType::InvalidRange
					},
					std::tuple
					{ 
						"index operator is called", 
						[ErrorOr](const DescriptorRange InRange){ return ErrorOr(InRange[0]); },
						DescriptorRange::EErrorType::InvalidRange
					}
				}
			)
		);

		GIVEN(given)
		{
			REQUIRE(range.GetSize() == expectedSize);
			REQUIRE(!range.IsValidRange());

			WHEN(when)
			{
				const auto result = errorOp(range);
				THEN("returns expected error")
				{
					REQUIRE(result == expectedError);
				}
			}
		}
	}

	GIVEN("Valid Range")
	{
		static constexpr u32 increment = 10;
		static constexpr u32 num = 20;
		static constexpr u32 expectedSize = increment * num;
		static constexpr u64 startAddress = 42;
		
		DescriptorRange range{ DescriptorHandle{{startAddress}, {startAddress}}, num, increment };

		REQUIRE(range.IsValidRange());

		WHEN("get first index")
		{
			const auto firstAddress = range.First()->GetCPUHandle().ptr;

			THEN("address is as startAddress")
			{
				REQUIRE(firstAddress == startAddress);
			}
		}

		WHEN("Get handle in range")
		{
			static constexpr u32 index = num - 1;
			const auto handle = range[index];

			THEN("address is as expected")
			{
				static constexpr u64 expectedAddress = startAddress + increment * index;

				REQUIRE(handle->GetCPUHandle().ptr == expectedAddress);
			}
		}

		WHEN("Get handle out of range")
		{
			static constexpr u32 index = num + 1;
			const auto handle = range[index];

			THEN("Invalid index error returned")
			{
				REQUIRE(!handle);
				REQUIRE(handle.error() == DescriptorRange::EErrorType::InvalidIndex);
			}
		}

		WHEN("Get index of handle in range")
		{
			static constexpr u32 expectedIndex = num - 1;
			static constexpr u64 address = startAddress + increment * expectedIndex;

			const auto maybeIndex = range.GetIndex(DescriptorHandle{ {address}, {address} });

			THEN("index is as expected")
			{
				REQUIRE(maybeIndex);
				REQUIRE(*maybeIndex == expectedIndex);
			}
		}

		WHEN("Get index of handle out of range")
		{
			static constexpr u32 expectedIndex = num + 1;
			static constexpr u64 address = startAddress + increment * expectedIndex;

			const auto maybeIndex = range.GetIndex(DescriptorHandle{ {address}, {address} });

			THEN("invalid handle error returned")
			{
				REQUIRE(!maybeIndex);
				REQUIRE(maybeIndex.error() == DescriptorRange::EErrorType::InvalidHandle);
			}
		}

		WHEN("Get sub range that is within range")
		{
			static constexpr u32 subStart = 1;
			static constexpr u32 subNum = num - 2;

			const auto subRange = range.Subrange(subStart, subNum);

			THEN("sub range is valid")
			{
				REQUIRE(subRange);
				REQUIRE(subRange->GetSize() == subNum);
			}
		}

		WHEN("Get sub range that is out of range")
		{
			static constexpr u32 subStart = num;
			static constexpr u32 subNum = num - 2;

			const auto subRange = range.Subrange(subStart, subNum);

			THEN("invalid sub range error returned")
			{
				REQUIRE(!subRange);
				REQUIRE(subRange.error() == DescriptorRange::EErrorType::InvalidSubrange);
			}
		}

	}
}
