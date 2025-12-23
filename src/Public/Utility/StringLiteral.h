
#pragma once

#include <string_view>

namespace stf
{
    class StringLiteral
    {
    public:

        consteval StringLiteral(const std::string_view InView)
            : m_View(InView)
        {
        }

        constexpr operator std::string_view() const
        {
            return View();
        }

        constexpr std::string_view View() const
        {
            return m_View;
        }

        friend bool operator==(const StringLiteral&, const StringLiteral&) = default;
        friend bool operator!=(const StringLiteral&, const StringLiteral&) = default;

    private:
        std::string_view m_View{};
    };
}