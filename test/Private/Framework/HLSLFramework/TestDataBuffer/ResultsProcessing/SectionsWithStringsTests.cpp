#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("HLSLFrameworkTests - TestDataBuffer - ResultProcessing - SectionsWithStrings")
{
    auto [testName, expectedStrings, expectedSections, expectedFailingSectionIds] = GENERATE
    (
        table<std::string, std::vector<std::string>, std::vector<STF::SectionInfoMetaData>, std::vector<i32>>
        (
            {
                std::tuple
                { 
                    "GIVEN_ZeroSectionsWithStrings_WHEN_ZeroAssertsFailed_THEN_HasExpectedResults",
                    std::vector<std::string>{},
                    std::vector<STF::SectionInfoMetaData>{},
                    std::vector<i32>{}
                },
                std::tuple
                { 
                    "GIVEN_ZeroSectionsWithStrings_WHEN_AssertsFailed_THEN_HasExpectedResults",
                    std::vector<std::string>{},
                    std::vector<STF::SectionInfoMetaData>{},
                    std::vector<i32>{-1}
                
                },
                std::tuple
                {
                    "GIVEN_OneScenario_WHEN_AssertsFailedInScenario_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        }
                    },
                    std::vector<i32>{0}
                },
                std::tuple
                {
                    "GIVEN_OneSection_WHEN_AssertsFailedInSection_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario", "TestSection"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 1,
                            .StringId = 1,
                            .ParentId = 0
                        }
                    },
                    std::vector<i32>{1}
                },
                std::tuple
                {
                    "GIVEN_OneSection_WHEN_AssertsFailedOutsideSection_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario", "TestSection"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 1,
                            .StringId = 1,
                            .ParentId = 0
                        }
                    },
                    std::vector<i32>{0}
                },
                std::tuple
                {
                    "GIVEN_TwoNonNestedSection_WHEN_AssertsFailedInFirstSection_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario", "TestSection1", "TestSection2"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 1,
                            .StringId = 1,
                            .ParentId = 0
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 2,
                            .StringId = 2,
                            .ParentId = 0
                        }
                    },
                    std::vector<i32>{1}
                },
                std::tuple
                {
                    "GIVEN_TwoNonNestedSection_WHEN_AssertsFailedInSecondSection_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario", "TestSection1", "TestSection2"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 1,
                            .StringId = 1,
                            .ParentId = 0
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 2,
                            .StringId = 2,
                            .ParentId = 0
                        }
                    },
                    std::vector<i32>{2}
                },
                std::tuple
                {
                    "GIVEN_NestedSections_WHEN_AssertsInEachSection_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario", "TestSection1", "TestSection2", "TestSection3", "TestSection4"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 1,
                            .StringId = 1,
                            .ParentId = 0
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 2,
                            .StringId = 2,
                            .ParentId = 1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 3,
                            .StringId = 3,
                            .ParentId = 1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 4,
                            .StringId = 4,
                            .ParentId = 0
                        }
                    },
                    std::vector<i32>{1,2,1,3,4}
                },
                std::tuple
                {
                    "GIVEN_NestedSectionsWithMultipleThreads_WHEN_AssertsInEachSection_THEN_HasExpectedResults",
                    std::vector<std::string>{"TestScenario", "TestSection1", "TestSection2", "TestSection3", "TestSection4"},
                    std::vector
                    {
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 0,
                            .StringId = 0,
                            .ParentId = -1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 1,
                            .StringId = 1,
                            .ParentId = 0
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 2,
                            .StringId = 2,
                            .ParentId = 1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 3,
                            .StringId = 3,
                            .ParentId = 1
                        },
                        STF::SectionInfoMetaData
                        {
                            .SectionId = 4,
                            .StringId = 4,
                            .ParentId = 0
                        }
                    },
                    std::vector<i32>{1,2,1,3,4}
                }
            }
        )
    );

    ShaderTestFixture::Desc desc{};

    desc.Mappings.emplace_back(GetTestVirtualDirectoryMapping());
    desc.Source = fs::path("/Tests/TestDataBuffer/ResultsProcessing/SectionsWithStrings.hlsl");
    desc.GPUDeviceParams.DeviceType = GPUDevice::EDeviceType::Software;
    desc.TestDataLayout = STF::TestDataBufferLayout(100u, 10000u, 50u, 1000u, 50u);

    ShaderTestFixture fixture{std::move(desc)};
    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(testName, 1, 1, 1);
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        
        REQUIRE(actual->FailedAsserts.size() == expectedFailingSectionIds.size());
        REQUIRE(actual->Sections.size() == expectedSections.size());
        REQUIRE(actual->Strings.size() == expectedStrings.size());
        REQUIRE(actual->Sections == expectedSections);
        REQUIRE(actual->Strings == expectedStrings);

        for (u64 failedIndex = 0; failedIndex < expectedFailingSectionIds.size(); ++failedIndex)
        {
            REQUIRE(actual->FailedAsserts[failedIndex].Info.SectionId == expectedFailingSectionIds[failedIndex]);
        }
    }
}