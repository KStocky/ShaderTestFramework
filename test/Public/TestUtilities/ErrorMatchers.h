#pragma once

#include <Utility/Error.h>
#include <catch2/matchers/catch_matchers_templated.hpp>

namespace stf
{
    class ErrorContainsMatcher 
        : public ::Catch::Matchers::MatcherGenericBase
    {
    public:

        ErrorContainsMatcher(const ErrorFragment& InError)
            : m_ErrorFragment{ InError }
        {
        }

        ErrorContainsMatcher(ErrorFragment&& InError)
            : m_ErrorFragment{ std::move(InError) }
        {
        }

        bool match(const Error& InError) const
        {
            return InError.HasFragment(m_ErrorFragment);
        }

        std::string describe() const override
        {
            return std::format("contains fragment: {}", m_ErrorFragment);
        }

    private:
        ErrorFragment m_ErrorFragment;
    };

    class ErrorContainsFormatMatcher
        : public ::Catch::Matchers::MatcherGenericBase
    {
    public:

        ErrorContainsFormatMatcher(StringLiteral&& InFormat)
            : m_Format{ std::move(InFormat) }
        {
        }

        ErrorContainsFormatMatcher(const StringLiteral& InFormat)
            : m_Format{ InFormat }
        {
        }

        bool match(const Error& InError) const
        {
            return InError.HasFragmentWithFormat(m_Format);
        }

        std::string describe() const override
        {
            return std::format("contains fragment with format: {}", m_Format.View());
        }

    private:
        StringLiteral m_Format;
    };

    inline ErrorContainsMatcher ErrorContains(const ErrorFragment& InError)
    {
        return ErrorContainsMatcher{ InError };
    }

    inline ErrorContainsMatcher ErrorContains(ErrorFragment&& InError)
    {
        return ErrorContainsMatcher{ std::move(InError) };
    }

    inline ErrorContainsFormatMatcher ErrorContainsFormat(const StringLiteral& InFormat)
    {
        return ErrorContainsFormatMatcher{ InFormat };
    }

    inline ErrorContainsFormatMatcher ErrorContainsFormat(StringLiteral&& InFormat)
    {
        return ErrorContainsFormatMatcher{ std::move(InFormat) };
    }
}