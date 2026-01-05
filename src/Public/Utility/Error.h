
#pragma once

#include "Platform.h"

#include "Utility/Concepts.h"
#include "Utility/Expected.h"
#include "Utility/FixedString.h"
#include "Utility/StringLiteral.h"

#include <algorithm>
#include <format>
#include <iterator>
#include <ostream>
#include <ranges>
#include <vector>

namespace stf
{
    class ErrorFragmentToken
    {
        friend class ErrorFragment;
        constexpr ErrorFragmentToken() = default;
    };

    class ErrorFragment
    {
    public:

        ErrorFragment() = delete;

        constexpr ErrorFragment(ErrorFragmentToken, StringLiteral InFormat, std::string InFragment)
            : m_Format(InFormat)
            , m_Fragment(std::move(InFragment))
        {
        }

        template<FixedString InFormat>
        constexpr static ErrorFragment Make()
        {
            return ErrorFragment{
                ErrorFragmentToken{},
                StringLiteral(InFormat.Data),
                InFormat.Data
            };
        }

        template<FixedString InFormat, typename... ArgTypes>
            requires (Formattable<ArgTypes, char> && ...)
        static ErrorFragment Make(ArgTypes&&... InArgs)
        {
            return ErrorFragment{
                ErrorFragmentToken{},
                StringLiteral(InFormat.Data),
                std::format(InFormat.Data, std::forward<ArgTypes>(InArgs)...)
            };
        }

        friend bool operator==(const ErrorFragment&, const ErrorFragment&) = default;
        friend bool operator!=(const ErrorFragment&, const ErrorFragment&) = default;

        bool HasSameFormat(const ErrorFragment& In) const
        {
            return HasFormat(In.Format());
        }

        bool HasFormat(const StringLiteral In) const
        {
            return m_Format == In;
        }

        StringLiteral Format() const
        {
            return m_Format;
        }

        std::string_view Error() const
        {
            return m_Fragment;
        }

    private:
        StringLiteral m_Format;
        std::string m_Fragment{};
    };

    class Error
    {
    public:

        Error() = default;

        explicit Error(ErrorFragment&& InFragment)
        {
            Append(std::move(InFragment));
        }

        template<FixedString InFormat>
        static Error FromFragment()
        {
            return Error{ ErrorFragment::Make<InFormat>() };
        }

        template<FixedString InFormat, typename... ArgTypes>
            requires (Formattable<ArgTypes, char> && ...)
        static Error FromFragment(ArgTypes&&... InArgs)
        {
            return Error{ ErrorFragment::Make<InFormat>(std::forward<ArgTypes>(InArgs)...) };
        }

        void Append(ErrorFragment&& InFragment)
        {
            m_Fragments.push_back(std::move(InFragment));
        }

        void Append(const ErrorFragment& InFragment)
        {
            m_Fragments.push_back((InFragment));
        }

        bool HasFragmentWithFormat(const StringLiteral InFormat) const
        {
            return std::ranges::any_of(
                m_Fragments, 
                [&](const ErrorFragment& InFragment) 
                { 
                    return InFragment.HasFormat(InFormat);
                });
        }

        template<std::output_iterator<const char&> OutType>
        auto FormatTo(OutType InIterator) const
        {
            for (const auto& fragment : m_Fragments | std::views::reverse)
            {
                std::ranges::copy(fragment.Error(), InIterator);
                *InIterator++ = '\n';
            }
            return InIterator;
        }

        Error& operator+=(const ErrorFragment& InFragment)
        {
            Append(InFragment);
            return *this;
        }

        Error& operator+=(ErrorFragment&& InFragment)
        {
            Append(std::move(InFragment));
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& InOutStream, const Error& InError);

        friend bool operator==(const Error&, const Error&) = default;
        friend bool operator!=(const Error&, const Error&) = default;

    private:

        std::vector<ErrorFragment> m_Fragments{};
    };

    template<typename T>
    using ExpectedError = Expected<T, Error>;
}

template<>
struct std::formatter<stf::ErrorFragment> : std::formatter<string_view> {
    auto format(const stf::ErrorFragment& In, auto& ctx) const {
        return std::formatter<string_view>::format(In.Error(), ctx);
    }
};

template<>
struct std::formatter<stf::Error> : std::formatter<string_view> {
    auto format(const stf::Error& In, auto& ctx) const {
        return In.FormatTo(ctx.out());
    }
};

namespace stf
{
    inline std::ostream& operator<<(std::ostream& InOutStream, const Error& InError)
    {
        std::print(InOutStream, "{}", InError);
        return InOutStream;
    }
}