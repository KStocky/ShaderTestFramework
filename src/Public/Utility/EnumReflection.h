
#pragma once
#include <array>
#include <concepts>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace stf
{
    namespace Private
    {
        template<typename T>
        concept EnumType = std::is_enum_v<T>;

        template<typename T>
        concept ScopedEnum = EnumType<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;

        template<typename... T>
        struct AllSameTrait
        {
            static constexpr bool Value = true;
        };

        template<typename T, typename... U>
        struct AllSameTrait<T, U...>
        {
            static constexpr bool Value = (std::is_same_v<T, U> && ...);
        };

        template<typename... T>
        concept AllSame = AllSameTrait<T...>::Value;

        template<typename... T>
        concept AllSameEnum = AllSame<T...> && (... && EnumType<T>);

        template<typename T, typename... U>
        using FirstType = T;

        template<EnumType T, T Val>
        consteval std::string_view EnumValueToString() noexcept
        {
            constexpr std::string_view funcSig = __FUNCSIG__;
            constexpr std::size_t end = funcSig.find_last_of('>');

            constexpr bool NotFromTemplateFunction = end == std::string_view::npos;

            if (NotFromTemplateFunction)
            {
                return {};
            }

            constexpr std::size_t begin = [funcSig, end]()
                {
                    for (size_t i = end; i > 0; --i)
                    {
                        const auto charIndex = i - 1;
                        if (funcSig[charIndex] == ')')
                        {
                            return std::string_view::npos;
                        }

                        if (funcSig[charIndex] == ',')
                        {
                            return funcSig.find_first_not_of(' ', charIndex + 1);
                        }
                    }

                    return std::string_view::npos;
                }();

            if constexpr (begin == std::string_view::npos)
            {
                return {};
            }

            return funcSig.substr(begin, end - begin);
        }

        template<EnumType T, int32_t InMin>
        consteval T EnumUnderlyingValueToValue(const int32_t InUnderlyingValue) noexcept
        {
            return static_cast<T>(InUnderlyingValue + InMin);
        }

        template<EnumType T, T Val>
        consteval bool IsValidEnumValue() noexcept
        {
            return !EnumValueToString<T, Val>().empty();
        }

        template<EnumType T, T Val, typename PredType>
        consteval bool IsValidEnumValue(const PredType InPred) noexcept
        {
            constexpr auto enumString = EnumValueToString<T, Val>();
            return !enumString.empty() && InPred(enumString);
        }

        template<EnumType T, int32_t InDefaultMin, int32_t InMax, std::size_t... I>
        consteval int32_t FindMinValImpl(std::index_sequence<I...>) noexcept
        {
            constexpr std::size_t rangeSize = (InMax - InDefaultMin) + 1;
            constexpr std::array<bool, rangeSize> validArray{ IsValidEnumValue<T, EnumUnderlyingValueToValue<T, InDefaultMin>(I)>()... };
            for (std::size_t i = 0; i < rangeSize; ++i)
            {
                if (validArray[i])
                {
                    return static_cast<int32_t>(i) + InDefaultMin;
                }
            }

            return InDefaultMin;
        }

        template<EnumType T, int32_t InDefaultMin, int32_t InMax, typename Indices = std::make_index_sequence<(InMax - InDefaultMin) + 1>>
        consteval int32_t FindMinVal() noexcept
        {
            return FindMinValImpl<T, InDefaultMin, InMax>(Indices{});
        }

        template<EnumType T, int32_t InMin, int32_t InDefaultMax, std::size_t... I>
        consteval int32_t FindMaxValImpl(std::index_sequence<I...>) noexcept
        {
            constexpr std::size_t rangeSize = (InDefaultMax - InMin) + 1;
            static_assert(rangeSize > 0, "Enum needs to have more than 0 values");

            constexpr std::array<bool, rangeSize> validArray{ IsValidEnumValue<T, EnumUnderlyingValueToValue<T, InMin>(I)>()... };
            for (int32_t i = static_cast<int32_t>(rangeSize) - 1; i >= 0; --i)
            {
                if (validArray[i])
                {
                    return static_cast<int32_t>(i) + InMin;
                }
            }

            return InDefaultMax;
        }

        template<EnumType T, int32_t InMin, int32_t InDefaultMax, typename Indices = std::make_index_sequence<(InDefaultMax - InMin) + 1>>
        consteval int32_t FindMaxVal() noexcept
        {
            return FindMaxValImpl<T, InMin, InDefaultMax>(Indices{});
        }


        template<std::size_t InRangeSize>
        consteval int32_t CountValid(const std::array<bool, InRangeSize>& InRange) noexcept
        {
            int32_t ret = 0;

            for (std::size_t i = 0; i < InRangeSize; ++i)
            {
                if (InRange[i])
                {
                    ++ret;
                }
            }

            return ret;
        }

        template<EnumType T, std::size_t InArraySize, std::array<T, InArraySize> InValues, std::size_t... I>
        consteval auto ArrayOfValuesToArrayOfNames(std::index_sequence<I...>) noexcept
        {
            return std::array<std::string_view, InArraySize>{EnumValueToString<T, InValues[I]>()...};
        }

        template<EnumType T, int32_t InMin, int32_t InMax, std::size_t... I>
        consteval auto GetEnumValues(std::index_sequence<I...>) noexcept
        {
            constexpr std::size_t rangeSize = (InMax - InMin) + 1;
            constexpr std::array<bool, rangeSize> validArray{ IsValidEnumValue<T, EnumUnderlyingValueToValue<T, InMin>(I)>()... };

            constexpr std::size_t validSize = CountValid(validArray);

            std::array<T, validSize> validValues{};

            for (std::size_t i = 0, j = 0; i < rangeSize; ++i)
            {
                if (validArray[i])
                {
                    validValues[j++] = static_cast<T>(i + InMin);
                }
            }
            return validValues;
        }

        template<EnumType T, int32_t InMin, int32_t InMax, typename PredType, std::size_t... I>
        consteval auto GetEnumValues(std::index_sequence<I...>, const PredType InPred) noexcept
        {
            constexpr std::size_t rangeSize = (InMax - InMin) + 1;
            constexpr std::array<bool, rangeSize> validArray{ IsValidEnumValue<T, EnumUnderlyingValueToValue<T, InMin>(I)>(InPred)... };

            constexpr std::size_t validSize = CountValid(validArray);

            std::array<T, validSize> validValues{};

            for (std::size_t i = 0, j = 0; i < rangeSize; ++i)
            {
                if (validArray[i])
                {
                    validValues[j++] = static_cast<T>(i + InMin);
                }
            }
            return validValues;
        }

        template<EnumType T, int32_t InMin, int32_t InMax, std::size_t... I>
        consteval auto GetUnderlyingEnumValues(std::index_sequence<I...>) noexcept
        {
            constexpr std::size_t rangeSize = (InMax - InMin) + 1;
            constexpr std::array<bool, rangeSize> validArray{ IsValidEnumValue<T, EnumUnderlyingValueToValue<T, InMin>(I)>()... };

            constexpr std::size_t validSize = CountValid(validArray);

            std::array<std::underlying_type_t<T>, validSize> validValues{};

            for (std::size_t i = 0, j = 0; i < rangeSize; ++i)
            {
                if (validArray[i])
                {
                    validValues[j++] = static_cast<std::underlying_type_t<T>>(i + InMin);
                }
            }
            return validValues;
        }

        template<EnumType T, std::size_t InNumEnums, std::array<T, InNumEnums> InEnumValues>
        constexpr std::optional<std::size_t> GetIndexOfEnumValue(const T InVal) noexcept
        {
            for (std::size_t i = 0; i < InNumEnums; ++i)
            {
                if (InEnumValues[i] == InVal)
                {
                    return i;
                }
            }

            return std::optional<std::size_t>{};
        }

        template<std::integral T>
        constexpr auto Min(T&& InA)
        {
            return InA;
        }

        template<std::integral T, std::integral U, std::integral... V>
        constexpr auto Min(T&& InA, U&& InB, V&&... InOthers)
        {
            if constexpr (sizeof...(InOthers) > 0)
            {
                return InA <= InB ? Min(std::forward<T>(InA), std::forward<V>(InOthers)...) : Min(std::forward<U>(InB), std::forward<V>(InOthers)...);
            }
            else
            {
                return InA <= InB ? InA : InB;
            }
        }

        template<std::integral T>
        constexpr auto Max(T&& InA)
        {
            return InA;
        }

        template<std::integral T, std::integral U, std::integral... V>
        constexpr auto Max(T&& InA, U&& InB, V&&... InOthers)
        {
            if constexpr (sizeof...(InOthers) > 0)
            {
                return InA > InB ? Max(std::forward<T>(InA), std::forward<V>(InOthers)...) : Max(std::forward<U>(InB), std::forward<V>(InOthers)...);
            }
            else
            {
                return InA > InB ? InA : InB;
            }
        }

        template<EnumType... T> requires AllSame<T...>
        constexpr auto FirstEnumVal(T&&... InVals) noexcept
        {
            return Min(static_cast<std::underlying_type_t>(InVals)...);
        }

        template<EnumType... T> requires AllSame<T...>
        constexpr auto LastEnumVal(T&&... InVals) noexcept
        {
            return Max(static_cast<std::underlying_type_t>(InVals)...);
        }

        template<std::integral T>
        constexpr bool IsPowerOf2(const T In) noexcept
        {
            return In == 0 || (In & (In - 1)) == 0;
        }

        template<EnumType T, std::size_t InArraySize, std::array<T, InArraySize> InValues>
        constexpr bool IsFlags() noexcept
        {
            for (const auto value : InValues)
            {
                const auto underlying = static_cast<std::underlying_type_t<T>>(value);
                const auto isPowerOf2 = IsPowerOf2(underlying);
                if (!isPowerOf2)
                {
                    return false;
                }
            }

            return true;
        }

        static constexpr int32_t DefaultMinEnumValue = -128;
        static constexpr int32_t DefaultMaxEnumValue = 127;
    }

    namespace Enum
    {
        using namespace Private;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        struct EnumReflect
        {
            using Enum = T;
            using UnderlyingType = std::underlying_type_t<Enum>;
            static constexpr bool IsSigned = std::is_unsigned_v<UnderlyingType>;
            static constexpr bool IsScoped = ScopedEnum<Enum>;

            static constexpr int32_t MinValue = FindMinVal<T, InMin, InMax>();
            static constexpr int32_t MaxValue = FindMaxVal<T, MinValue, InMax>();
            static constexpr int32_t RangeSize = (MaxValue - MinValue) + 1;
            static constexpr auto EnumValuesArray = GetEnumValues<Enum, MinValue, MaxValue>(std::make_index_sequence<RangeSize>{});
            static constexpr auto UnderlyingEnumValuesArray = GetUnderlyingEnumValues<Enum, MinValue, MaxValue>(std::make_index_sequence<RangeSize>{});
            static constexpr auto NumEnums = EnumValuesArray.size();
            static constexpr auto EnumNamesArray = ArrayOfValuesToArrayOfNames<Enum, NumEnums, EnumValuesArray>(std::make_index_sequence<NumEnums>{});
            static constexpr auto IsFlags = Private::IsFlags<Enum, NumEnums, EnumValuesArray>();

            static constexpr std::string_view FullName(const Enum InVal) noexcept
            {
                const auto index = GetIndexOfEnumValue<Enum, NumEnums, EnumValuesArray>(InVal);
                return index ? EnumNamesArray[*index] : std::string_view{};
            }

            static constexpr std::string_view UnscopedName(const Enum InVal) noexcept
            {
                const auto fullName = FullName(InVal);
                const auto lastColon = fullName.find_last_of(':');
                const auto begin = lastColon == std::string_view::npos ? 0 : lastColon + 1;

                return fullName.substr(begin);
            }

            template<typename PredType>
            static constexpr auto GetValues(const PredType InPred) noexcept
            {
                return GetEnumValues<Enum, MinValue, MaxValue>(std::make_index_sequence<RangeSize>{}, InPred);
            }

            template<std::size_t InArraySize>
            static std::unordered_map<std::string_view, Enum> GenerateMapUnscopedNameToValue(const std::array<Enum, InArraySize>& InValues)
            {
                std::unordered_map<std::string_view, Enum> ret;
                for (std::size_t i = 0; i < InValues.size(); ++i)
                {
                    ret.insert({ UnscopedName(InValues[i]), InValues[i] });
                }

                return ret;
            }

            static constexpr std::string_view ScopedName(const Enum InVal) noexcept
            {
                if constexpr (!IsScoped)
                {
                    return UnscopedName(InVal);
                }

                const auto fullName = FullName(InVal);
                constexpr size_t maxNumColons = 2;

                const size_t firstColonPos = fullName.find_last_of(':');
                const size_t thirdColonPos = fullName.find_last_of(':', firstColonPos - maxNumColons);

                const size_t begin = thirdColonPos == std::string_view::npos ? 0 : thirdColonPos + 1;

                return fullName.substr(begin);
            }

            static constexpr std::optional<T> ToEnum(const std::string_view InName) noexcept
            {
                const auto index = [&]() constexpr -> std::optional<std::size_t>
                    {
                        for (std::size_t i = 0; i < NumEnums; ++i)
                        {
                            if (EnumNamesArray[i] == InName)
                            {
                                return i;
                            }
                        }

                        return std::optional<std::size_t>{};
                    }();

                return index ? EnumValuesArray[*index] : std::optional<T>{};
            }
        };

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr bool IsSigned = EnumReflect<T, InMin, InMax>::IsSigned;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr bool IsScoped = EnumReflect<T, InMin, InMax>::IsScoped;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr int32_t MinValue = EnumReflect<T, InMin, InMax>::MinValue;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr int32_t MaxValue = EnumReflect<T, InMin, InMax>::MaxValue;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr int32_t RangeSize = EnumReflect<T, InMin, InMax>::RangeSize;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr auto EnumValuesArray = EnumReflect<T, InMin, InMax>::EnumValuesArray;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr auto UnderlyingEnumValuesArray = EnumReflect<T, InMin, InMax>::UnderlyingEnumValuesArray;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr auto NumEnums = EnumReflect<T, InMin, InMax>::NumEnums;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr auto EnumNamesArray = EnumReflect<T, InMin, InMax>::EnumNamesArray;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        inline constexpr auto IsFlags = EnumReflect<T, InMin, InMax>::IsFlags;

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        constexpr std::string_view FullName(const T InVal) noexcept
        {
            return EnumReflect<T, InMin, InMax>::FullName(InVal);
        }

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        constexpr std::string_view UnscopedName(const T InVal) noexcept
        {
            return EnumReflect<T, InMin, InMax>::UnscopedName(InVal);
        }

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        constexpr std::string_view ScopedName(const T InVal) noexcept
        {
            return EnumReflect<T, InMin, InMax>::ScopedName(InVal);
        }

        template<EnumType T, T... InList>
        constexpr std::string_view FullNameFromList(const T InVal) noexcept
        {
            static constexpr std::array<T, sizeof...(InList)> values{ InList... };
            static constexpr std::array<std::string_view, sizeof...(InList)> names{ EnumValueToString<T, InList>()... };

            for (size_t index = 0; index < values.size(); ++index)
            {
                if (InVal == values[index])
                {
                    return names[index];
                }
            }
            return "";
        }

        template<EnumType T>
        constexpr auto ToUnderlyingType(const T InEnum) noexcept
        {
            return static_cast<std::underlying_type_t<T>>(InEnum);
        }

        template<EnumType T, T... InList>
        constexpr std::string_view UnscopedNameFromList(const T InVal) noexcept
        {
            const auto fullName = FullNameFromList<T, InList...>(InVal);
            const auto lastColon = fullName.find_last_of(':');
            const auto begin = lastColon == std::string_view::npos ? 0 : lastColon + 1;

            return fullName.substr(begin);
        }

        template<EnumType T, T... InList>
        constexpr std::string_view ScopedNameFromList(const T InVal) noexcept
        {
            static constexpr bool isScoped = ScopedEnum<T>;
            if constexpr (!isScoped)
            {
                return UnscopedNameFromList<T, InList...>(InVal);
            }

            const auto fullName = FullNameFromList<T, InList...>(InVal);
            constexpr size_t maxNumColons = 2;

            const size_t firstColonPos = fullName.find_last_of(':');
            const size_t thirdColonPos = fullName.find_last_of(':', firstColonPos - 2);

            const size_t begin = thirdColonPos == std::string_view::npos ? 0 : thirdColonPos + 1;

            return fullName.substr(begin);
        }

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue>
        constexpr std::optional<T> ToEnum(const std::string_view InName) noexcept
        {
            return EnumReflect<T, InMin, InMax>::ToEnum(InName);
        }

        template<EnumType T, int32_t InMin = DefaultMinEnumValue, int32_t InMax = DefaultMaxEnumValue, typename PredType>
        constexpr auto GetValues(const PredType InPred) noexcept
        {
            return EnumReflect<T, InMin, InMax>::GetValues(InPred);
        }

        template<EnumType... FlagTypes> requires AllSame<FlagTypes...> && (sizeof...(FlagTypes) > 0)
            constexpr auto MakeFlags(const FlagTypes... InFlags) noexcept
        {
            using RetType = FirstType<FlagTypes...>;
            const auto underlying = (static_cast<std::underlying_type_t<RetType>>(InFlags) | ...);

            return static_cast<RetType>(underlying);
        }

        template<EnumType T>
        constexpr bool EnumHasMask(const T InVal, const T InMask) noexcept
        {
            return (static_cast<std::underlying_type_t<T>>(InVal) & static_cast<std::underlying_type_t<T>>(InMask)) == static_cast<std::underlying_type_t<T>>(InMask);
        }

        template<EnumType T>
        constexpr bool EnumHasMaskNotSet(const T InVal, const T InMask) noexcept
        {
            return (static_cast<std::underlying_type_t<T>>(InVal) & static_cast<std::underlying_type_t<T>>(InMask)) == 0;
        }

        template<EnumType T>
        constexpr bool EnumHasAtLeastOneFlagSet(const T InVal, const T InMask) noexcept
        {
            return (static_cast<std::underlying_type_t<T>>(InVal) & static_cast<std::underlying_type_t<T>>(InMask)) > 0;
        }

        template<EnumType T1, EnumType T2>
        constexpr bool EnumsAreEquivilentRanges() noexcept
        {
            using CommonType = std::common_type_t<std::underlying_type_t<T1>, std::underlying_type_t<T2>>;

            const auto range1 = UnderlyingEnumValuesArray<T1>;
            const auto range2 = UnderlyingEnumValuesArray<T2>;

            if (range1.size() != range2.size())
            {
                return false;
            }

            for (size_t index = 0; index < range1.size(); ++index)
            {
                if (static_cast<CommonType>(range1[index]) != static_cast<CommonType>(range2[index]))
                {
                    return false;
                }
            }

            return true;
        }
    }
}