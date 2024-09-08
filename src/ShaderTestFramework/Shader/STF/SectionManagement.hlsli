#ifndef STF_SECTION_MANAGEMENT_HEADER
#define STF_SECTION_MANAGEMENT_HEADER

#include "/Test/STF/ByteReaderTraits.hlsli"
#include "/Test/TTL/byte_writer.hlsli"
#include "/Test/TTL/caster.hlsli"
#include "/Test/TTL/memory.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace stf
{
    namespace detail
    {
        static const int NumSections = 32;

        enum class ESectionRunState
        {
            NeverEntered,
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
            int NextStringID;
            ThreadIDInfo ThreadID;
            ScenarioSectionInfo Sections[NumSections];

            int GetSectionID()
            {
                return Sections[0].ParentID == -1 ? CurrentSectionID : -1;
            }

            void Init()
            {
                NextSectionID = 1;
                Sections[0].ParentID = -1;
            }

            void OnLeave(int InID)
            {
                const bool hasUnenteredSubsections = Sections[InID].RunState == ESectionRunState::RunningNeedsRerun;
                Sections[InID].RunState = hasUnenteredSubsections ? ESectionRunState::NeedsRun : ESectionRunState::Completed;
                CurrentSectionID = Sections[InID].ParentID;
                Sections[CurrentSectionID].RunState = hasUnenteredSubsections ?
                    ESectionRunState::RunningNeedsRerun :
                    Sections[CurrentSectionID].RunState;
        
            }

            void OnLeave()
            {
                OnLeave(CurrentSectionID);
            }

            bool ShouldEnter(int InID)
            {   
                const ESectionRunState state = Sections[InID].RunState;
                switch (state)
                {
                    case ESectionRunState::NeverEntered:
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

            template<typename T>
            bool TryLoopScenario(const T InOnFirstEnter)
            {
                if (Sections[0].RunState == ESectionRunState::NeverEntered)
                {
                    InOnFirstEnter(0, -1);
                }
                const bool shouldEnter =
                    Sections[0].RunState == ESectionRunState::NeverEntered ||
                    Sections[0].RunState == ESectionRunState::NeedsRun || 
                    Sections[0].RunState == ESectionRunState::RunningNeedsRerun;

                if (shouldEnter)
                {
                    CurrentSectionID = 0;
                    Sections[0].RunState = ESectionRunState::Running;
                    
                    return true;
                }
            
                return false;
            }

            template<typename T>
            bool TryEnterSection(const T InOnFirstEnter, int InID)
            {
                if (Sections[InID].RunState == ESectionRunState::NeverEntered)
                {
                    Sections[InID].ParentID = CurrentSectionID;
                    InOnFirstEnter(InID, CurrentSectionID);
                    Sections[InID].RunState = ESectionRunState::NeedsRun;
                }

                if (!ShouldEnter(InID))
                {
                    return false;
                }

                const bool ourTurn = Sections[CurrentSectionID].RunState == ESectionRunState::Running;
                if (ourTurn)
                {
                    Sections[CurrentSectionID].RunState = ESectionRunState::RunningEnteredSubsection;
                    Sections[InID].RunState = ESectionRunState::Running;
                    CurrentSectionID = InID;
                    return true;
                }
                else
                {   
                    Sections[CurrentSectionID].RunState = ESectionRunState::RunningNeedsRerun;  
                    return false;
                }
            }
        };
    
        struct SectionHierarchy
        {
            PerThreadScratchData Scratch;
        };

        static PerThreadScratchData Scratch;

        uint FlattenIndex(const uint3 InIndex, const uint3 InDimensions)
        {
            return (InIndex.z * InDimensions.x * InDimensions.y) + (InIndex.y * InDimensions.x) + InIndex.x;
        }
    }
}

namespace ttl
{
    using stf::detail::PerThreadScratchData;
    using stf::detail::SectionHierarchy;

    template<>
    struct byte_writer<SectionHierarchy>
    {
        static const bool has_writer = true;

        static uint bytes_required(SectionHierarchy In)
        {
            if (In.Scratch.Sections[0].ParentID != -1)
            {
                return 0;
            }

            uint numSections = 1;
            int currentSection = In.Scratch.CurrentSectionID;
            while(In.Scratch.Sections[currentSection].ParentID != -1)
            {
                ++numSections;
                currentSection = In.Scratch.Sections[currentSection].ParentID;
            }
            return ttl::aligned_offset(numSections, 4u);
        }

        static uint alignment_required(SectionHierarchy In)
        {
            return ttl::align_of_v<uint>;
        }

        template<typename U>
        static void write(inout container_wrapper<U> InContainer, const uint InIndex, const SectionHierarchy In)
        {
            const uint numUints = bytes_required(In) / ttl::size_of_v<uint>;
            static const bool isByteAddress = ttl::container_traits<U>::is_byte_address;
            static const uint storeIndexModifier = isByteAddress ? 4 : 1;

            int currentSection = In.Scratch.CurrentSectionID;

            for (uint i = 0; i < numUints; ++i)
            {
                uint packedIds = 0;
                
                for (uint numPackedSectionIds = 0; numPackedSectionIds < 4 && currentSection != -1; ++numPackedSectionIds)
                {
                    packedIds = packedIds | ((uint)currentSection << (numPackedSectionIds * 8u));
                    currentSection = In.Scratch.Sections[currentSection].ParentID;
                }

                InContainer.store(InIndex + i * storeIndexModifier, packedIds);
            }
        }
    };
}

namespace stf
{
    template<>
    struct ByteReaderTraits<detail::PerThreadScratchData> : ByteReaderTraitsBase<READER_ID_PER_THREAD_SCRATCH>
    {
    };
}

namespace ttl
{
    template<>
    struct caster<bool, stf::detail::PerThreadScratchData>
    {
        static bool cast(stf::detail::PerThreadScratchData In)
        {
            return false;
        }
    };
}

#endif
