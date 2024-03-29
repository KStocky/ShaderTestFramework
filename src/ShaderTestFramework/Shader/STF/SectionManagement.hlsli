#pragma once

#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/type_traits.hlsli"

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

namespace ttl
{
    using ShaderTestPrivate::PerThreadScratchData;

    template<>
    struct byte_writer<PerThreadScratchData>
    {
        static const bool has_writer = true;

        static uint bytes_required(PerThreadScratchData In)
        {
            if (In.Sections[0].ParentID != -1)
            {
                return 0;
            }

            uint numSections = 1;
            int currentSection = In.CurrentSectionID;
            while(In.Sections[currentSection].ParentID != -1)
            {
                ++numSections;
                currentSection = In.Sections[currentSection].ParentID;
            }
            return ttl::aligned_offset(numSections, 4u);
        }

        static uint alignment_required(PerThreadScratchData In)
        {
            return ttl::align_of<uint>::value;
        }

        template<typename U>
        static void write(inout container_wrapper<U> InContainer, const uint InIndex, const PerThreadScratchData In)
        {
            const uint numUints = bytes_required(In) / ttl::size_of<uint>::value;
            static const bool isByteAddress = ttl::container_traits<U>::is_byte_address;
            static const uint storeIndexModifier = isByteAddress ? 4 : 1;

            int currentSection = In.CurrentSectionID;

            for (uint i = 0; i < numUints; ++i)
            {
                uint packedIds = 0;
                
                for (uint numPackedSectionIds = 0; numPackedSectionIds < 4 && currentSection != -1; ++numPackedSectionIds)
                {
                    packedIds = packedIds | (currentSection << (numPackedSectionIds * 8));
                    currentSection = In.Sections[currentSection].ParentID;
                }

                InContainer.store(InIndex + i * storeIndexModifier, packedIds);
            }
        }
    };
}
