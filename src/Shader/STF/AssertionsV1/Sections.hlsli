#ifndef STF_ASSERTIONSV1_SECTIONS_HEADER
#define STF_ASSERTIONSV1_SECTIONS_HEADER

#include "/Test/STF/AssertionsV1/Strings.hlsli"
#include "/Test/STF/SectionManagement.hlsli"

namespace stf
{
    namespace AssertionsV1
    {
        namespace detail
        {
            template<uint N>
            void AddGlobalString(const int InId, ttl::string<N> In)
            {
                const uint stringIndex = (uint)InId;
                if (!ShouldWriteGlobalData(NumStringsIndex, InId))
                {
                    return;
                }

                if (stringIndex < Strings.Num())
                {
                    uint2 addressAndSize = uint2(0, 0);
                    if (Strings.SizeInBytesOfData() > 0)
                    {
                        addressAndSize = AddStringData(In);
                    }

                    AddStringMetaInfo(stringIndex, addressAndSize);
                }
            }

            void AddGlobalSection(SectionInfoMetaData InSectionInfo)
            {
                const uint sectionIndex = (uint)InSectionInfo.SectionId;
                if (!ShouldWriteGlobalData(NumSectionsIndex, InSectionInfo.SectionId))
                {
                    return;
                }

                if (sectionIndex < Sections.Num())
                {
                    const uint bufferAddress = Sections.BeginMeta() + sectionIndex * sizeof(SectionInfoMetaData);
                    GetTestDataBuffer().Store(bufferAddress, InSectionInfo);
                }
            }

            struct OnFirstEntryOfSectionFunctor
            {
                int StringId;
                void operator()(const int InSectionId, const int InParentId)
                {
                    SectionInfoMetaData metaData;
                    metaData.SectionId = InSectionId;
                    metaData.ParentId = InParentId;
                    metaData.StringId = StringId;
                    AddGlobalSection(metaData);
                }
            };
        }
    }
}

#define STF_GET_SECTION_VAR_NAME(InID) STF_Section_##InID##_Var
#define STF_CREATE_SECTION_VAR_IMPL(InID) \
    static const int STF_GET_SECTION_VAR_NAME(InID) = stf::detail::Scratch.NextSectionID++ \

#define STF_CREATE_SECTION_VAR STF_CREATE_SECTION_VAR_IMPL(__LINE__)

#define STF_GET_TEST_FUNC_NAME(InID) STF_TEST_FUNC_##InID
#define STF_DECLARE_TEST_FUNC(InID) void STF_GET_TEST_FUNC_NAME(InID)()

#define STF_DEFINE_TEST_ENTRY_FUNC(InID, InName, ...) void InName(__VA_ARGS__) \
{\
    stf::detail::Scratch.Init();\
    STF_DECLARE_TEST_FUNC(InID); \
    while(stf::detail::Scratch.TryLoopScenario())\
    {\
        STF_GET_TEST_FUNC_NAME(InID)();\
    }\
}\
STF_DECLARE_TEST_FUNC(InID)

#define STF_SCENARIO_IMPL(InName, InScenarioId)                                                                                   \
CREATE_STRING(TTL_JOIN(scenarioNameString, InScenarioId), InName);                                                         \
static int TTL_JOIN(scenarioNameId, InScenarioId) = stf::detail::Scratch.NextStringID++; \
stf::AssertionsV1::detail::AddGlobalString(TTL_JOIN(scenarioNameId, InScenarioId), TTL_JOIN(scenarioNameString, InScenarioId));    \
stf::AssertionsV1::detail::OnFirstEntryOfSectionFunctor TTL_JOIN(onFirstEntry, InScenarioId);                                                                       \
TTL_JOIN(onFirstEntry, InScenarioId).StringId = TTL_JOIN(scenarioNameId, InScenarioId);                                                                     \
stf::detail::Scratch.Init();                                                                                                  \
while(stf::detail::Scratch.TryLoopScenario(TTL_JOIN(onFirstEntry, InScenarioId)))

#define SCENARIO(InName) STF_SCENARIO_IMPL(InName, __LINE__)

#define STF_SECTION_IMPL(InName, InID) STF_CREATE_SECTION_VAR_IMPL(InID);                                       \
CREATE_STRING(TTL_JOIN(sectionNameString, InID), InName);                                              \
static int TTL_JOIN(sectionNameId, InID) = stf::detail::Scratch.NextStringID++; \
stf::AssertionsV1::detail::AddGlobalString(TTL_JOIN(sectionNameId, InID), TTL_JOIN(sectionNameString, InID)); \
stf::AssertionsV1::detail::OnFirstEntryOfSectionFunctor TTL_JOIN(onFirstEntry, InID);                                  \
TTL_JOIN(onFirstEntry, InID).StringId = TTL_JOIN(sectionNameId, InID);                                        \
while (stf::detail::Scratch.TryEnterSection(TTL_JOIN(onFirstEntry, InID), STF_GET_SECTION_VAR_NAME(InID)))

#define SECTION(InName) STF_SECTION_IMPL(InName, __LINE__)

#endif