
#pragma once
#include "Platform.h"

#include "Utility/Concepts.h"

#include <chrono>

namespace stf
{
    template<ArithmeticType T>
    using Seconds = std::chrono::duration<T>;

    template<ArithmeticType T>
    using Milliseconds = std::chrono::duration<T, std::milli>;

    template<ArithmeticType T>
    using Microseconds = std::chrono::duration<T, std::micro>;

    template<ArithmeticType T>
    using Nanoseconds = std::chrono::duration<T, std::nano>;

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
}
