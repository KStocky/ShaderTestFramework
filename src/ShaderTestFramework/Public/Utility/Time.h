
#include "Platform.h"

#include <chrono>

using SecondsF = std::chrono::duration<f32>;
using MillisecondsF = std::chrono::duration<f32, std::ratio<1, 1000>>;
using MicrosecondsF = std::chrono::duration<f32, std::ratio<1, 1000000>>;
using NanosecondsF = std::chrono::duration<f32, std::ratio<1, 1000000000>>;
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

class Duration
{
public:

    constexpr Duration() = default;

    constexpr Duration(Clock::duration InDuration)
        : m_Duration(InDuration)
    {
    }

    constexpr Duration(SecondsF InSeconds)
        : m_Duration(std::chrono::duration_cast<Clock::duration>(InSeconds))
    {
    }

    constexpr Duration(MillisecondsF InMilliseconds)
        : m_Duration(std::chrono::duration_cast<Clock::duration>(InMilliseconds))
    {
    }

    constexpr operator SecondsF() const
    {
        return std::chrono::duration_cast<SecondsF>(m_Duration);
    }

    constexpr operator MillisecondsF() const
    {
        return std::chrono::duration_cast<MillisecondsF>(m_Duration);
    }

    constexpr operator MicrosecondsF() const
    {
        return std::chrono::duration_cast<MicrosecondsF>(m_Duration);
    }

    constexpr operator NanosecondsF() const
    {
        return std::chrono::duration_cast<NanosecondsF>(m_Duration);
    }

    constexpr operator Clock::duration() const
    {
        return m_Duration;
    }

    template<typename T>
    constexpr auto Count() const
    {
        return std::chrono::duration_cast<T>(m_Duration).count();
    }

private:

    Clock::duration m_Duration{};
};