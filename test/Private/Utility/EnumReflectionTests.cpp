#include "Platform.h"
#include "Utility/EnumReflection.h"

#include <string>

namespace EnumReflectionTests
{
    using namespace stf;
	enum class ETestScoped
	{
		One = -23,
		Two,
		Three = 45
	};

	enum ETestUnscoped
	{
		One = -23,
		Two,
		Three = 45
	};

	enum class ETestAllFlags
	{
		One = 1,
		Two = 2,
		Three = 4,
		Four = 8,
		Five = 16
	};

	enum class ETestSomeFlags
	{
		StartFlags = 0,
		One = 1,
		Two = 2,
		Three = 4,
		EndFlags,
		Four,
		Five,
		Six
	};

	namespace ScopedEnumTests
	{
		namespace MinValue
		{

			static_assert(-23 == Enum::MinValue<ETestScoped>);
		}

		namespace MaxValue
		{

			static_assert(45 == Enum::MaxValue<ETestScoped>);
		}

		namespace IsScoped
		{
			static_assert(Enum::IsScoped<ETestScoped>);
		}

		namespace Names
		{
			static constexpr auto names = Enum::EnumNamesArray<ETestScoped>;
			static_assert(3ull == names.size());
		}

		namespace UnscopedName_ValidEnumValue_ReturnsExpectedString
		{
			static constexpr std::string_view expected = "Two";
			static constexpr auto actual = Enum::UnscopedName(ETestScoped::Two);
			static_assert(expected == actual);
		}

		namespace UnscopedName_InvalidEnumValue_ReturnsEmptyString
		{
			static constexpr std::string_view expected = "";
			static constexpr auto actual = Enum::UnscopedName(static_cast<ETestScoped>(0));
			static_assert(expected == actual);
		}

		namespace ScopedName_ValidEnumValue_ReturnsExpectedString
		{
			constexpr std::string_view expected = "ETestScoped::Two";
			constexpr auto actual = Enum::ScopedName(ETestScoped::Two);
			static_assert(expected == actual);
		}

		namespace ScopedName_InvalidEnumValue_ReturnsEmptyString
		{
			constexpr std::string_view expected = "";
			constexpr auto actual = Enum::ScopedName(static_cast<ETestScoped>(0));
			static_assert(expected == actual);
		}

		namespace FullName_ValidEnumValue_ReturnsExpectedString
		{
			constexpr std::string_view expected = "EnumReflectionTests::ETestScoped::Two";
			constexpr auto actual = Enum::FullName(ETestScoped::Two);
			static_assert(expected == actual);
		}

		namespace FullName_InvalidEnumValue_ReturnsEmptyString
		{
			constexpr std::string_view expected = "";
			constexpr auto actual = Enum::FullName(static_cast<ETestScoped>(0));
			static_assert(expected == actual);
		}

		namespace ToEnum_ValidString_ReturnsExpectedValue
		{
			constexpr std::string_view name = "EnumReflectionTests::ETestScoped::Three";
			constexpr i32 expected = static_cast<i32>(ETestScoped::Three);
			constexpr auto actual = Enum::ToEnum<ETestScoped>(name);
			static_assert(!!actual);

			static_assert(expected == static_cast<i32>(*actual));
		}

		namespace ToEnum_InvalidString_ReturnsEmptyOptional
		{
			constexpr std::string_view name = "ETestScoped::Three";
			constexpr auto actual = Enum::ToEnum<ETestScoped>(name);
			static_assert(!actual);
		}

		namespace GetValues_WithPredicateToRemoveValue_ArrayDoesntHaveValue
		{
			using Reflect = Enum::EnumReflect<ETestScoped>;

			static constexpr auto numEnums = Reflect::NumEnums;

			static constexpr auto expectedMissingValue = ETestScoped::Two;
			static constexpr auto expectedMissingName = Enum::UnscopedName(expectedMissingValue);
			static constexpr std::size_t expectedNumValues = numEnums - 1;
			static constexpr auto values = Reflect::GetValues([](const std::string_view InName) constexpr { return InName.find(expectedMissingName) == std::string_view::npos; });

			static_assert(expectedNumValues == values.size(), "Expected there to be one fewer value than members of the enum");

			constexpr bool Test()
			{
				for (const auto& value : values)
				{
					if (value == expectedMissingValue)
					{
						return false;
					}
				}

				return true;
			}

			static_assert(Test());
		}
	}

	namespace UnscopedEnumTests
	{
		namespace MinValue
		{
			static_assert(-23 == Enum::MinValue<ETestUnscoped>);
		}

		namespace MaxValue
		{

			static_assert(45 == Enum::MaxValue<ETestUnscoped>);
		}

		namespace IsScoped
		{
			static_assert(!Enum::IsScoped<ETestUnscoped>);
		}

		namespace Names
		{
			static constexpr auto names = Enum::EnumNamesArray<ETestUnscoped>;
			static_assert(3ull == names.size());
		}

		namespace UnscopedName_ValidEnumValue_ReturnsExpectedString
		{
			static constexpr std::string_view expected = "Two";
			static constexpr auto actual = Enum::UnscopedName(ETestUnscoped::Two);
			static_assert(expected == actual);
		}

		namespace UnscopedName_InvalidEnumValue_ReturnsEmptyString
		{
			constexpr std::string_view expected = "";
			constexpr auto actual = Enum::UnscopedName(static_cast<ETestUnscoped>(0));
			static_assert(expected == actual);
		}

		namespace ScopedName_ValidEnumValue_ReturnsExpectedString
		{
			constexpr std::string_view expected = "Two";
			constexpr auto actual = Enum::ScopedName(ETestUnscoped::Two);
			static_assert(expected == actual);
		}

		namespace ScopedName_InvalidEnumValue_ReturnsEmptyString
		{
			constexpr std::string_view expected = "";
			constexpr auto actual = Enum::ScopedName(static_cast<ETestUnscoped>(0));
			static_assert(expected == actual);
		}

		namespace FullName_ValidEnumValue_ReturnsExpectedString
		{
			static constexpr std::string_view expected = "EnumReflectionTests::Two";
			static constexpr auto actual = Enum::FullName(ETestUnscoped::Two);
			static_assert(expected == actual);
		}

		namespace FullName_InvalidEnumValue_ReturnsEmptyString
		{
			constexpr std::string_view expected = "";
			constexpr auto actual = Enum::FullName(static_cast<ETestUnscoped>(0));
			static_assert(expected == actual);
		}

		namespace ToEnum_ValidString_ReturnsExpectedValue
		{
			constexpr std::string_view name = "EnumReflectionTests::Three";
			constexpr int32_t expected = static_cast<int32_t>(ETestUnscoped::Three);
			constexpr auto actual = Enum::ToEnum<ETestUnscoped>(name);
			static_assert(actual.has_value());

			static_assert(expected == static_cast<i32>(*actual));
		}

		namespace ToEnum_InvalidString_ReturnsEmptyOptional
		{
			constexpr std::string_view name = "EnumReflectionTests::Three";
			constexpr auto actual = Enum::ToEnum<ETestUnscoped>(name);
			static_assert(!!actual);
		}
	}

	namespace FlaggedEnumTests
	{
		namespace IsFlags_EnumOnlyContainsFlags_ReturnsTrue
		{

			static_assert(Enum::IsFlags<ETestAllFlags>);
		}

		namespace IsFlags_RangeContainsOnlyFlags_ReturnsTrue
		{
			static_assert(Enum::IsFlags<ETestSomeFlags, static_cast<i32>(ETestSomeFlags::StartFlags) + 1, static_cast<i32>(ETestSomeFlags::EndFlags) - 1>);
		}

		namespace IsFlags_RangeContainsMixOfFlagsAndNoFlags_ReturnsFalse
		{
			static_assert(!Enum::IsFlags<ETestSomeFlags>);
		}

		namespace IsFlags_RangeContainsNoFlags_ReturnsFalse
		{
			static_assert(!Enum::IsFlags<ETestSomeFlags, static_cast<i32>(ETestSomeFlags::EndFlags) + 1>);
		}

		namespace IsFlags_EnumContainsNoFlags_ReturnsFalse
		{
			static_assert(!Enum::IsFlags<ETestScoped>);
		}

		namespace MakeFlags_SingleFlagArg_ReturnsFlag
		{
			static constexpr auto expected = ETestAllFlags::Two;
			static_assert(expected == Enum::MakeFlags(expected));
		}

		namespace MakeFlags_TwoFlagArgs_ReturnsCombination
		{
			static constexpr auto expected = static_cast<ETestAllFlags>(static_cast<i32>(ETestAllFlags::One) | static_cast<i32>(ETestAllFlags::Three));
			static_assert(expected == Enum::MakeFlags(ETestAllFlags::One, ETestAllFlags::Three));
		}

		namespace EnumHasMask_TwoFlagsSetAndOneCompared_ReturnsTrue
		{
			static constexpr auto flags = Enum::MakeFlags(ETestAllFlags::One, ETestAllFlags::Three);
			static constexpr auto mask = ETestAllFlags::One;

			static_assert(Enum::EnumHasMask(flags, mask));
		}

		namespace EnumHasMask_TwoFlagsSetAndTwoCompared_ReturnsTrue
		{
			static constexpr auto flags = Enum::MakeFlags(ETestAllFlags::One, ETestAllFlags::Three);

			static_assert(Enum::EnumHasMask(flags, flags));
		}

		namespace EnumHasMask_TwoFlagsSetAndThreeCompared_ReturnsFalse
		{
			static constexpr auto flags = Enum::MakeFlags(ETestAllFlags::One, ETestAllFlags::Three);
			static constexpr auto mask = Enum::MakeFlags(ETestAllFlags::One, ETestAllFlags::Two, ETestAllFlags::Three);

			static_assert(!Enum::EnumHasMask(flags, mask));
		}
	}

	namespace MiscTests
	{

		enum class EEqualRangeA
		{
			A = 5,
			B = 7,
			C = 15
		};

		enum class EEqualRangeB
		{
			D = 5,
			E = 7,
			F = 15
		};

		enum class ENotEqualRange
		{
			A,
			B,
			C
		};

		namespace EnumsAreEqualRanges_EnumRangesAreEqual_ReturnsTrue
		{
			static_assert(Enum::EnumsAreEquivilentRanges<EEqualRangeA, EEqualRangeB>());
		}

		namespace EnumsAreEqualRanges_EnumRangesAreNotEqual_ReturnsFalse
		{
			static_assert(!Enum::EnumsAreEquivilentRanges<EEqualRangeA, ENotEqualRange>());
		}

	}
}
