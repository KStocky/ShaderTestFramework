#include "Stats/StatSystem.h"
#include <ranges>

StatSystem GlobalStatSystem;

StatSystem::Handle StatSystem::BeginStat(std::string InName)
{
    if (m_RawTimedStats.size() > Handle::MaxIds)
    {
        return Handle{ 0u, 0u, false };
    }
    const u32 id = static_cast<u32>(m_RawTimedStats.size());

    m_RawTimedStats.push_back(RawTimedStat{ std::move(InName), Clock::now() });

    return Handle{id, m_CurrentGen, true};
}

void StatSystem::EndStat(const Handle InHandle)
{
    if (InHandle && InHandle.GetGeneration() == m_CurrentGen && InHandle.GetId() < m_RawTimedStats.size())
    {
        auto& rawStat = m_RawTimedStats[InHandle.GetId()];
        rawStat.EndTime = Clock::now();
    }
}

std::vector<TimedStat> StatSystem::FlushTimedStats()
{
    std::vector<TimedStat> ret;

    for (auto&& completed : std::views::all(m_RawTimedStats)
                            | std::views::filter(
                                [](const RawTimedStat& In) 
                                { 
                                    return In.EndTime.time_since_epoch().count() != 0; 
                                }))
    {
        ret.push_back(TimedStat{ std::move(completed.Name), completed.EndTime - completed.StartTime });
    }

    m_RawTimedStats.clear();
    m_CurrentGen = (m_CurrentGen + 1) % Handle::MaxGens;

    return ret;
}

