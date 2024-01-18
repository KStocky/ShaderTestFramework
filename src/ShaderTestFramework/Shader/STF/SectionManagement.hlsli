#pragma once

namespace ShaderTestPrivate
{
    static const int NumSections = 32;

    enum class ESectionRunState
    {
        NeedsRun,
        Running,
        RunningEnteredSubsection,
        RunningNeedsRerun,
        Completed
    };

    struct ScenarioSectionInfo
    {
        int ParentID;
        ESectionRunState RunState; 
    };

    enum class EThreadIDType
    {
        None,
        Int,
        Int3
    };

    struct ThreadIDInfo
    {
        uint Data;
        EThreadIDType Type;
    };
    
    struct PerThreadScratchData
    {
        int CurrentSectionID;
        int NextSectionID;
        ThreadIDInfo ThreadID;
        ScenarioSectionInfo Sections[NumSections];
    };
    
    static PerThreadScratchData Scratch;

    void InitScratch()
    {
        Scratch.NextSectionID = 1;
        Scratch.Sections[0].ParentID = -1;
    }

    void OnLeave(int InID = Scratch.CurrentSectionID)
    {
        const bool hasUnenteredSubsections = Scratch.Sections[InID].RunState == ESectionRunState::RunningNeedsRerun;
        Scratch.Sections[InID].RunState = hasUnenteredSubsections ? ESectionRunState::NeedsRun : ESectionRunState::Completed;
        Scratch.CurrentSectionID = Scratch.Sections[InID].ParentID;
        Scratch.Sections[Scratch.CurrentSectionID].RunState = hasUnenteredSubsections ?
            ESectionRunState::RunningNeedsRerun :
            Scratch.Sections[Scratch.CurrentSectionID].RunState;
    
    }

     bool ShouldEnter(int InID)
    {   
        const ESectionRunState state = Scratch.Sections[InID].RunState;
        switch (state)
        {
            case ESectionRunState::NeedsRun:
            {
                return true;
            }
            case ESectionRunState::RunningNeedsRerun:
            case ESectionRunState::RunningEnteredSubsection:
            case ESectionRunState::Running:
            {
                OnLeave(InID);
                return false;
            }
            case ESectionRunState::Completed:
            {
                return false;
            }
        }

        return false;
    }

    bool TryLoopScenario()
    {
        const bool shouldEnter = 
            Scratch.Sections[0].RunState == ESectionRunState::NeedsRun || 
            Scratch.Sections[0].RunState == ESectionRunState::RunningNeedsRerun;

        if (shouldEnter)
        {
            Scratch.CurrentSectionID = 0;
            Scratch.Sections[0].RunState = ESectionRunState::Running;
                
            return true;
        }
        
        return false;
    }

    bool TryEnterSection(int InID)
    {
        if (!ShouldEnter(InID))
        {
            return false;
        }

        const bool ourTurn = Scratch.Sections[Scratch.CurrentSectionID].RunState == ESectionRunState::Running;
        if (ourTurn)
        {
            Scratch.Sections[Scratch.CurrentSectionID].RunState = ESectionRunState::RunningEnteredSubsection;
            Scratch.Sections[InID].ParentID = Scratch.CurrentSectionID;
            Scratch.Sections[InID].RunState = ESectionRunState::Running;
            Scratch.CurrentSectionID = InID;
            return true;
        }
        else
        {   
            Scratch.Sections[Scratch.CurrentSectionID].RunState = ESectionRunState::RunningNeedsRerun;  
            return false;
        }
    }

    uint FlattenIndex(const uint3 InIndex, const uint3 InDimensions)
    {
        return (InIndex.z * InDimensions.x * InDimensions.y) + (InIndex.y * InDimensions.x) + InIndex.x;
    }
}
