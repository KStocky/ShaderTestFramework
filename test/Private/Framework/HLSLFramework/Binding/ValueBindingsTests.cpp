
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <D3D12/Shader/ShaderBindingMap.h>
#include <Framework/ShaderTestFixture.h>
#include <Utility/Expected.h>
#include <Utility/HLSLTypes.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

class ValueBindingsFixture
    : public ShaderTestFixtureBaseFixture
{
public:

    ValueBindingsFixture()
        : ShaderTestFixtureBaseFixture(
            stf::ShaderTestFixture::FixtureDesc
            {
                .Mappings{ GetTestVirtualDirectoryMapping() },
                .GPUDeviceParams
                {
                    .DebugLevel = stf::GPUDevice::EDebugLevel::DebugLayer,
                    .DeviceType = stf::GPUDevice::EDeviceType::Software,
                    .EnableGPUCapture = false
                }
            }
        )
    {
    }
};

TEST_CASE_PERSISTENT_FIXTURE(ValueBindingsFixture, "HLSLFrameworkTests - Bindings - ValueBindings")
{
    using namespace stf;

    struct GlobalBindingsStruct
    {
        float A{ 4.0f };
        i32 B{ 42 };
        int2 Padding1{};
        float4 C{ 1.0f, 2.0f, 3.0f, 4.0f };
    };

    struct TooLargeStruct
    {
        float2 D;
        float2 Padding1;
        int3 E{42, 2, 4};

        i32 F[16]{};
        i32 G[16]{};
        i32 H[16]{};
    };

    auto [testName, testFile, bindings, expectedResult] = GENERATE
    (
        table<std::string, std::string, std::vector<ShaderBinding>, ExpectedError<bool>>
        (
            {
                std::tuple
                {
                    "Valid Global Bindings with Expected Values",
                    "GlobalBindings",
                    std::vector<ShaderBinding>
                    {
                        { "MyParam", GlobalBindingsStruct{} },
                        { "D", float2{5.0f, 6.0f}},
                        { "E", int3{123, 456, 789}}
                    },
                    true
                },
                std::tuple
                {
                    "Valid Global Bindings with Unexpected Values",
                    "GlobalBindings",
                    std::vector<ShaderBinding>
                    {
                        { "MyParam", GlobalBindingsStruct{} },
                        { "D", float2{5.0f, 6.0f}},
                        { "E", int3{0, 456, 789}}
                    },
                    false
                },
                std::tuple
                {
                    "Pass data that is too large for Binding",
                    "GlobalBindings",
                    std::vector<ShaderBinding>
                    {
                        { "MyParam", GlobalBindingsStruct{} },
                        { "D", float3{5.0f, 6.0f, 7.0f}},
                        { "E", int3{123, 456, 789}}
                    },
                    Unexpected{ Errors::BindingIsSmallerThanBindingData("D", sizeof(float2), sizeof(float3))}
                },
                std::tuple
                {
                    "Specify invalid bindings",
                    "GlobalBindings",
                    std::vector<ShaderBinding>
                    {
                        { "MyParam", GlobalBindingsStruct{} },
                        { "D", float2{5.0f, 6.0f}},
                        { "E", int3{123, 456, 789}},
                        { "F", i32{ 234 }}
                    },
                    Unexpected{ Errors::BindingDoesNotExist("F") }
                },
                std::tuple
                {
                    "Too Many Parameters for Root sig constants",
                    "GlobalBindingsTooLarge",
                    std::vector<ShaderBinding>
                    {
                        { "Param1", GlobalBindingsStruct{} },
                        { "Param2", TooLargeStruct{}}
                    },
                    true
                },
                std::tuple
                {
                    "Params contains an array",
                    "WithArray",
                    std::vector<ShaderBinding>
                    {
                        {
                            "Param", std::array{
                                2.0f, 0.0f, 0.0f, 0.0f,
                                42.0f
                            }
                        }
                    },
                    true
                },
                std::tuple
                {
                    "Multiple constant buffers with expected values",
                    "MultipleConstantBuffers",
                    std::vector<ShaderBinding>
                    {
                        {"FirstParam", GlobalBindingsStruct{.A = 2.0f, .B = 24, .C{5.0f, 6.0f, 7.0f, 8.0f}}},
                        {"SecondParam", GlobalBindingsStruct{.A = 102.5f, .B = 4195, .C{5.0f, 10.0f, 15.0f, 28.5f}}}
                    },
                    true
                },
                std::tuple
                {
                    "Multiple constant buffers with unexpected values",
                    "MultipleConstantBuffers",
                    std::vector<ShaderBinding>
                    {
                        {"FirstParam", GlobalBindingsStruct{.A = 4.0f, .B = 24, .C{5.0f, 6.0f, 7.0f, 8.0f}}},
                        {"SecondParam", GlobalBindingsStruct{.A = 102.5f, .B = 4195, .C{5.0f, 10.0f, 15.0f, 28.5f}}}
                    },
                    false
                },
                std::tuple
                {
                    "Array of constant buffers",
                    "ConstantBufferArray",
                    std::vector<ShaderBinding>
                    {
                        {"Buffs", std::array{4.0f, 4.0f}}
                    },
                    Unexpected{ Errors::ConstantBufferMustBeBoundToDecriptorTable("Buffs")}
                },
                std::tuple
                {
                    "Params have alignment of less than 4 bytes",
                    "BindingsHave2ByteAlignment",
                    std::vector<ShaderBinding>
                    {
                        {"Param1", u16{2}},
                        {"Param2", u16{2}},
                        {"Param3", u16{2}},
                        {"Param4", u16{2}}
                    },
                    true
                }
            }
        )
    );

    const auto getDesc =
        [&testFile, &bindings]()
        {
            return
                ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path(std::format("/Tests/Binding/ValueBindingsTests/{}.hlsl", testFile))
                },
                .TestName = "Main",
                .Bindings = std::move(bindings),
                .ThreadGroupCount{1, 1, 1}
            };
        };

    DYNAMIC_SECTION(testName)
    {
        const auto actual = fixture.RunTest(getDesc());
        if (expectedResult.has_value())
        {
            CAPTURE(actual);
            
            const bool testResult = actual;
            REQUIRE(testResult == expectedResult.value());
        }
        else
        {
            const auto results = actual.GetTestRunError();
            REQUIRE(results);
            REQUIRE(*results == expectedResult.error());
        }
    }
}