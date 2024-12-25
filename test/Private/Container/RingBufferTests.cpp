
#include <Container/RingBuffer.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>


SCENARIO("RingBufferTests")
{
    using namespace stf;
	enum class EState
	{
		Constructed,
		CopyConstructed,
		CopyAssigned,
		MoveConstructed,
		MoveAssigned,
		MovedFrom
	};

	struct MoveableType
	{
		MoveableType()
			: State(EState::Constructed)
		{

		}

		MoveableType(int InNum)
			: State(EState::Constructed)
			, Num(InNum)
		{

		}

		MoveableType(const MoveableType& In)
			: State(EState::CopyConstructed)
			, Num(In.Num)
		{}

		MoveableType(MoveableType&& In) noexcept
			: State(EState::MoveConstructed)
			, Num(In.Num)
		{
			In.State = EState::MovedFrom;
		}

		MoveableType& operator=(const MoveableType& In)
		{
			State = EState::CopyAssigned;
			Num = In.Num;
			return *this;
		}

		MoveableType& operator=(MoveableType&& In) noexcept
		{
			In.State = EState::MovedFrom;
			State = EState::MoveAssigned;
			Num = In.Num;

			return *this;
		}

		EState State;
		int Num = 0;
	};

	GIVEN("Default constructed")
	{
		constexpr i64 expected = 0;
		RingBuffer<MoveableType> buffer;

		WHEN("Size queried")
		{
			const i64 actual = buffer.size();
			THEN("Size is zero")
			{
				REQUIRE(expected == actual);
			}
		}
	}

	GIVEN("Zero Capacity")
	{
		constexpr u64 capacity = 0;
		RingBuffer<MoveableType> buffer(capacity);

		WHEN("Size queried")
		{
			const i64 actual = buffer.size();

			THEN("Size is zero")
			{
				static constexpr i64 expected = 0;
				REQUIRE(expected == actual);
			}
		}

		WHEN("an item is pushed back")
		{
			static constexpr i64 expected = 1;
			buffer.push_back(expected);

			THEN("buffer contains item")
			{
				REQUIRE(expected == buffer.size());
				REQUIRE(expected == buffer.front().Num);
			}
		}

		WHEN("iterated on")
		{
			u32 actual = 0;
			for (const auto element : buffer)
			{
				++actual;
			}

			THEN("zero iterations")
			{
				static constexpr u32 expected = 0;
				REQUIRE(expected == actual);
			}
		}

        WHEN("resized to same size")
        {
            static constexpr u64 newCapacity = capacity;
            const auto resizeResult = buffer.resize(newCapacity);

            THEN("succeeds")
            {
                REQUIRE(resizeResult.has_value());
                REQUIRE(newCapacity == buffer.capacity());
                REQUIRE(0 == buffer.size());
            }
        }

        WHEN("resized to greater size")
        {
            static constexpr u64 newCapacity = capacity + 5;
            const auto resizeResult = buffer.resize(newCapacity);

            THEN("succeeds")
            {
                REQUIRE(resizeResult.has_value());
                REQUIRE(newCapacity == buffer.capacity());
                REQUIRE(0 == buffer.size());
            }
        }
	}

	GIVEN("Non zero capacity")
	{
		constexpr u64 size = 5;
		RingBuffer<MoveableType> buffer(size);

		WHEN("Size is queried")
		{
			const i64 actual = buffer.size();

			THEN("Size is zero")
			{
				constexpr i64 expected = 0;
				REQUIRE(expected == actual);
			}
		}

		WHEN("an item is pushed back")
		{
			static constexpr i32 expected = 1;
			buffer.push_back(MoveableType{expected});

			THEN("buffer contains item")
			{
				REQUIRE(expected == buffer.size());
				REQUIRE(expected == buffer.front().Num);
			}
		}

        WHEN("Resized to something smaller")
        {
            const auto resizeResult = buffer.resize(size - 1);

            THEN("fails")
            {
                REQUIRE_FALSE(resizeResult.has_value());
                REQUIRE(resizeResult.error() == RingBuffer<MoveableType>::EErrorType::AttemptedShrink);
            }
        }

        WHEN("resized to same size")
        {
            static constexpr u64 newCapacity = size;
            const auto resizeResult = buffer.resize(newCapacity);

            THEN("succeeds")
            {
                REQUIRE(resizeResult.has_value());
                REQUIRE(newCapacity == buffer.capacity());
                REQUIRE(0 == buffer.size());
            }
        }

        WHEN("resized to greater size")
        {
            static constexpr u64 newCapacity = size + 5;
            const auto resizeResult = buffer.resize(newCapacity);

            THEN("succeeds")
            {
                REQUIRE(resizeResult.has_value());
                REQUIRE(newCapacity == buffer.capacity());
                REQUIRE(0 == buffer.size());
            }
        }
	}

	GIVEN("two items exist")
	{
		static constexpr i32 firstValue = 2;
		static constexpr i32 secondValue = 3;

		constexpr u64 capacity = 5;
		RingBuffer<MoveableType> buffer(capacity);

		buffer.push_back(firstValue);
		buffer.push_back(secondValue);

		WHEN("first item popped")
		{
			const auto actual = buffer.pop_front();

			THEN("returns element")
			{
				static constexpr i64 expectedSize = 1;
				static constexpr i32 expectedValue = firstValue;
				static constexpr auto expectedState = EState::MoveConstructed;

				REQUIRE(expectedSize == buffer.size());
				REQUIRE(expectedValue == actual->Num);
				REQUIRE(expectedState == actual->State);
			}
		}

		WHEN("both items popped")
		{
			buffer.pop_front();
			const auto actual = buffer.pop_front();

			THEN("second element is as expected")
			{
				static constexpr i64 expectedSize = 0;
				static constexpr i32 expectedValue = secondValue;
				static constexpr auto expectedState = EState::MoveConstructed;

				REQUIRE(expectedSize == buffer.size());
				REQUIRE(expectedValue == actual->Num);
				REQUIRE(expectedState == actual->State);
			}
		}

		WHEN("iterated on")
		{
			u32 actual = 0;
			for (const auto element : buffer)
			{
				++actual;
			}
			
			THEN("two iterations")
			{
				static constexpr u32 expected = 2;
				REQUIRE(expected == actual);
			}
		}

        WHEN("Resized to something smaller")
        {
            const auto resizeResult = buffer.resize(capacity - 1);

            THEN("fails")
            {
                REQUIRE_FALSE(resizeResult.has_value());
                REQUIRE(resizeResult.error() == RingBuffer<MoveableType>::EErrorType::AttemptedShrink);
            }
        }

        WHEN("resized to same size")
        {
            static constexpr u64 newCapacity = capacity;
            const auto resizeResult = buffer.resize(newCapacity);

            THEN("succeeds")
            {
                REQUIRE(resizeResult.has_value());
                REQUIRE(newCapacity == buffer.capacity());
                REQUIRE(2 == buffer.size());
            }
        }

        WHEN("resized to greater size")
        {
            static constexpr u64 newCapacity = capacity + 5;
            const auto resizeResult = buffer.resize(newCapacity);

            THEN("succeeds")
            {
                REQUIRE(resizeResult.has_value());
                REQUIRE(newCapacity == buffer.capacity());
                REQUIRE(2 == buffer.size());
            }

            AND_WHEN("iterated on")
            {
                u32 actual = 0;
                for (const auto element : buffer)
                {
                    ++actual;
                }

                THEN("two iterations")
                {
                    static constexpr u32 expected = 2;
                    REQUIRE(expected == actual);
                }
            }
        }
	}
};
