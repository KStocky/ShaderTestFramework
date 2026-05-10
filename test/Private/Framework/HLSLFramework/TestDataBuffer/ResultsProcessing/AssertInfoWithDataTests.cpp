#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"
#include <Framework/AssertionsV1/ShaderTestFixture.h>
#include <Utility/Math.h>
#include <Utility/OverloadSet.h>
#include <Utility/Tuple.h>
#include <Utility/TypeTraits.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

class AssertInfoWithDataTestsFixture : public ShaderTestFixtureBaseFixture
{
public:
    AssertInfoWithDataTestsFixture()
        : ShaderTestFixtureBaseFixture(
            stf::AssertionsV1::ShaderTestFixture::FixtureDesc
            {
                .Mappings{ GetTestVirtualDirectoryMapping() }
            },
            []()
            {
                stf::AssertionsV1::AssertionsV1Interface assertionInterface;
                assertionInterface.RegisterByteReader("TEST_TYPE_WITH_WRITER", [](const stf::u16, const std::span<const std::byte>) { return ""; });
                return assertionInterface;
            }())
    {
    }
};

TEST_CASE_PERSISTENT_FIXTURE(AssertInfoWithDataTestsFixture, "HLSLFrameworkTests - TestDataBuffer - ResultProcessing - AssertInfoWithData")
{
    using namespace stf;
    using namespace stf::AssertionsV1;
    auto serializeImpl = OverloadSet{ 
        [] <typename T>(const T& InVal, std::vector<std::byte>& InOutBytes) -> std::enable_if_t<!TIsInstantiationOf<T, Tuple>::Value>
        {
            static constexpr u32 size = sizeof(T);
            static constexpr u32 align = alignof(T);
            static constexpr u32 sizeAndAlign = (size << 16) | align;
            static constexpr u64 sizeOfAllocation = AlignedOffset(size + 4, 8);

            const auto oldSize = InOutBytes.size();

            InOutBytes.resize(InOutBytes.size() + sizeOfAllocation);
            auto address = InOutBytes.data() + oldSize;
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &InVal, size);
        },
        [] <typename T>(const Tuple<T, T>& InVal, std::vector<std::byte>& InOutBytes)
        {
            static constexpr u32 size = sizeof(T);
            static constexpr u32 align = alignof(T);
            static constexpr u32 sizeAndAlign = (size << 16) | align;
            static constexpr u64 sizeSingle = AlignedOffset(size + 4, align);
            static constexpr u64 sizeOfAllocation = AlignedOffset(sizeSingle * 2, 8);

            const auto oldSize = InOutBytes.size();

            InOutBytes.resize(InOutBytes.size() + sizeOfAllocation);
            auto address = InOutBytes.data() + oldSize;
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &get<0>(InVal), size);
            address += AlignedOffset(size, 4);
            std::memcpy(address, &sizeAndAlign, sizeof(u32));
            address += AlignedOffset(sizeof(u32), align);
            std::memcpy(address, &get<1>(InVal), size);
        }
    };

    auto serialize = [&serializeImpl]<typename... T>(const T&... InVals)
    {
        std::vector<std::byte> ret;
        (serializeImpl(InVals, ret), ...);
        return ret;
    };

    static constexpr u32 expectedValueLeft = 34u;
    static constexpr u32 expectedValueRight = 12345678u;

    auto [testName, expected, numRecordedAsserts, numBytesData] = GENERATE_COPY
    (
        table<std::string, TestRunResults, u32, u32>
        (
            {
                std::tuple
                { 
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    TestRunResults
                    { 
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft), 
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42, 
                                    .ThreadId = uint3{},
                                }
                            }
                        }, 
                        .Strings = {}, 
                        .Sections = {}, 
                        .NumSucceeded = 0, 
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    { 
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft), 
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42, 
                                    .ThreadId = uint3{},
                                }
                            }
                        }, 
                        .Strings = {}, 
                        .Sections = {}, 
                        .NumSucceeded = 0, 
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft, expectedValueRight),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft, expectedValueRight),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft, expectedValueRight),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_FailedDoubleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueLeft, expectedValueRight),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 400
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdOrWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdNoWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndNotEnoughDataCapacity_WHEN_FailedSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 4
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            },
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeFailFirstThenSmallFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            },
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueRight),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            },
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallFailFirstThenLargeFailSingleAssertWithTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(expectedValueRight),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            },
                            FailedAssert
                            {
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 12
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SmallStructComparedWithLargerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(1u, uint3(1u, 2u, 3u)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                    std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_LargeStructComparedWithSmallerStructWithWriterAndEnoughCapacity_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(uint3(1u, 2u, 3u), 1u),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_OneLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            },
                            FailedAssert
                            {
                                .Data = serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoLargeFailDoubleAssertWithoutTypeIdWithWriter_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            },
                            FailedAssert
                            {
                                .Data = serialize(uint3(1000, 2000, 3000), uint3(4000, 5000, 6000)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                }
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_DoubleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u16, u16>{24u, 42u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u16, u16>{24u, 42u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            },
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u16, u16>{1024u, 4u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_DoubleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u64, u64>{24ul, 42ul}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u64, u64>{24ul, 42ul}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            },
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u64, u64>{1024u, 4u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertFirstAlign2SecondAlign8_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u16, u16>{24u, 42u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            },
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u64, u64>{1024u, 4u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoDoubleAssertFirstAlign8SecondAlign2_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u64, u64>{24ull, 42ull}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            },
                            FailedAssert
                            {
                                .Data = serialize(Tuple<u16, u16>{1024u, 4u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SingleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(static_cast<u16>(24u)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertOfTypesWithAlignment2_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(static_cast<u16>(24u)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            },
                            FailedAssert
                            {
                                .Data = serialize(static_cast<u16>(1024u)),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_SingleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(u64{24u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 1
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertOfTypesWithAlignment8_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(u64{24u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            },
                            FailedAssert
                            {
                                .Data = serialize(u64{1024u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertFirstAlign2SecondAlign8_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(u16{24u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            },
                            FailedAssert
                            {
                                .Data = serialize(u64{1024u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                },
                std::tuple
                {
                    "GIVEN_AssertInfoAndDataCapacity_WHEN_TwoSingleAssertFirstAlign8SecondAlign2_THEN_HasExpectedResults",
                    TestRunResults
                    {
                        .FailedAsserts =
                        {
                            FailedAssert
                            {
                                .Data = serialize(u64{24u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 10
                            },
                            FailedAssert
                            {
                                .Data = serialize(u16{1024u}),
                                .Info = AssertMetaData
                                {
                                    .LineNumber = 42,
                                    .ThreadId = uint3{},
                                },
                                .TypeId = 2
                            }
                        },
                        .Strings = {},
                        .Sections = {},
                        .NumSucceeded = 0,
                        .NumFailed = 2
                    },
                    10, 100
                }
            }
        )
    );

    DYNAMIC_SECTION(testName)
    {
        const auto results = fixture.RunTest(
            AssertionsV1::ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/Tests/TestDataBuffer/ResultsProcessing/AssertInfoWithData.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{1, 1, 1},
                .PerTestData
                {
                    .NumFailedAsserts = numRecordedAsserts,
                    .NumBytesAssertData = numBytesData,
                    .NumStrings = 0,
                    .NumBytesStringData = 0,
                    .NumSections = 0
                }
            }
        );
        CAPTURE(results);
        const auto actual = results.GetTestResults();
        REQUIRE(actual);
        REQUIRE(*actual == expected);
    }
}
