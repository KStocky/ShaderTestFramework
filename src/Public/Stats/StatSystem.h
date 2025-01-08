#pragma once
#include "Platform.h"
#include "Utility/Time.h"

#include <compare>
#include <concepts>
#include <string>
#include <vector>

namespace stf
{
    template<typename T>
    struct NamedStat
    {
        std::string Name;
        T Stat;
    };

    using TimedStat = NamedStat<Nanoseconds<i64>>;

    class StatSystem
    {
    public:

        class Handle
        {
        public:

            static constexpr u32 NumBitsId = 28;
            static constexpr u32 NumBitsGen = 3;
            static constexpr u32 ValidBits = 1;

            static constexpr u32 MaxIds = 1 << NumBitsId;
            static constexpr u32 MaxGens = 1 << NumBitsGen;

            Handle() = default;
            Handle(const u32 InId, const u32 InGeneration, const bool InIsValid)
                : m_Id(InId)
                , m_Gen(InGeneration)
                , m_IsValid(InIsValid)
            {}

            u32 GetId() const
            {
                return m_Id;
            }

            u32 GetGeneration() const
            {
                return m_Gen;
            }

            operator bool() const
            {
                return m_IsValid != 0;
            }

            friend auto operator<=>(Handle, Handle) = default;

        private:
            u32 m_Id : NumBitsId = 0;
            u32 m_Gen : NumBitsGen = 0;
            u32 m_IsValid : ValidBits = 0;
        };

        Handle BeginStat(std::string InName);
        void EndStat(const Handle InHandle);

        [[nodiscard]] std::vector<TimedStat> FlushTimedStats();

    private:

        struct RawTimedStat
        {
            std::string Name;
            TimePoint StartTime = {};
            TimePoint EndTime = {};
        };

        std::vector<RawTimedStat> m_RawTimedStats;
        u32 m_CurrentGen = 0u;
    };

    template<auto Callable>
    concept StatSystemGetter = requires
    {
        { Callable() } -> std::same_as<StatSystem&>;
    };

    template<auto Getter >
        requires StatSystemGetter<Getter>
    class ScopedCPUDurationStat
    {
    public:

        ScopedCPUDurationStat(std::string InName)
        {
            m_Handle = Getter().BeginStat(std::move(InName));
        }

        ~ScopedCPUDurationStat()
        {
            Getter().EndStat(m_Handle);
        }

        ScopedCPUDurationStat(const ScopedCPUDurationStat&) = delete;
        ScopedCPUDurationStat(ScopedCPUDurationStat&&) = delete;
        ScopedCPUDurationStat& operator=(const ScopedCPUDurationStat&) = delete;
        ScopedCPUDurationStat& operator=(ScopedCPUDurationStat&&) = delete;

        template<typename... Ts>
        static void* operator new(std::size_t, Ts&&...) = delete;

        template<typename... Ts>
        static void* operator new[](std::size_t, Ts&&...) = delete;

    private:

        StatSystem::Handle m_Handle{};
    };
}
