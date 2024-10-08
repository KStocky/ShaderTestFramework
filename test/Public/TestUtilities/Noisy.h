
#pragma once

#include <Platform.h>

#include <compare>
#include <ostream>
#include <unordered_map>

template<typename T>
concept NoisyLoggable = requires(stf::i32 EventId)
{
    { T::OnConstruction() };
    { T::OnCopyConstruction() };
    { T::OnCopyAssignment() };
    { T::OnMoveConstruction() };
    { T::OnMoveAssignment() };
    { T::OnDestruction() };
    { T::OnEvent(EventId) };
};

struct DefaultLogData
{
    std::unordered_map<stf::i32, stf::u64> NumEvents;
    stf::u64 NumConstructions = 0;
    stf::u64 NumCopyConstructions = 0;
    stf::u64 NumCopyAssignments = 0;
    stf::u64 NumMoveConstructions = 0;
    stf::u64 NumMoveAssignments = 0;
    stf::u64 NumDestructions = 0;

    friend auto operator<=>(const DefaultLogData&, const DefaultLogData&) = default;
};

std::ostream& operator<<(std::ostream& InStream, const DefaultLogData& InData);

struct DefaultNoisyLogger
{
    static DefaultLogData LogData;

    static void OnConstruction() { ++LogData.NumConstructions; }
    static void OnCopyConstruction() { ++LogData.NumCopyConstructions; }
    static void OnCopyAssignment() { ++LogData.NumCopyAssignments; }
    static void OnMoveConstruction() { ++LogData.NumMoveConstructions; }
    static void OnMoveAssignment() { ++LogData.NumMoveAssignments; }
    static void OnDestruction() { ++LogData.NumDestructions; }
    static void OnEvent(const stf::i32 InEventId) { ++LogData.NumEvents[InEventId]; }
};

template<NoisyLoggable LoggerType = DefaultNoisyLogger>
class Noisy
{
public:

    Noisy() { LoggerType::OnConstruction(); }
    Noisy(const Noisy&) { LoggerType::OnCopyConstruction(); }
    Noisy(Noisy&&) noexcept { LoggerType::OnMoveConstruction(); }
    Noisy& operator=(const Noisy&) { LoggerType::OnCopyAssignment(); return *this; }
    Noisy& operator=(Noisy&&) { LoggerType::OnMoveAssignment(); return *this; }
    ~Noisy() noexcept { LoggerType::OnDestruction(); }

    void Event(const stf::i32 InEventId) const { LoggerType::OnEvent(InEventId); }
};

using DefaultNoisy = Noisy<DefaultNoisyLogger>;