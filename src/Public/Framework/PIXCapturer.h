#pragma once

#include <string_view>

namespace stf
{
    class PIXCapturer
    {
    public:

        explicit PIXCapturer(const std::string_view InName, const bool InTakeCapture);
        PIXCapturer(const PIXCapturer&) = delete;
        PIXCapturer(PIXCapturer&&) = delete;
        PIXCapturer& operator=(const PIXCapturer&) = delete;
        PIXCapturer& operator=(PIXCapturer&&) = delete;
        ~PIXCapturer() noexcept;

    private:

        bool m_IsCapturing = false;
    };
}