
#include "Framework/HLSLFramework/HLSLFrameworkTestsCommon.h"

#include <Framework/HLSLTypes.h>
#include <Framework/ShaderTestFixture.h>
#include <Utility/Expected.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE_PERSISTENT_FIXTURE(ShaderTestFixtureBaseFixture, "HLSLFrameworkTests - Bindings - ValueBindings")
{
    using namespace stf;

    struct GlobalBindingsStruct
    {
        float A{ 4.0f };
        i32 B{ 42 };
        int2 Padding1;
        float4 C{ 1.0f, 2.0f, 3.0f, 4.0f };
    };

    auto [testName, testFile, bindings, expectedResult] = GENERATE
    (
        table<std::string, std::string, std::vector<ShaderBinding>, Expected<bool, ErrorTypeAndDescription>>
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
                    Unexpected{ ErrorTypeAndDescription {.Type = ETestRunErrorType::Binding } }
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
                    Unexpected{ ErrorTypeAndDescription { .Type = ETestRunErrorType::Binding } }
                },
                std::tuple
                {
                    "Too Many Parameters",
                    "GlobalBindingsTooLarge",
                    std::vector<ShaderBinding>
                    {
                        { "MyParam", GlobalBindingsStruct{} },
                        { "D", float2{5.0f, 6.0f}},
                        { "E", int3{123, 456, 789}},
                        { "F", std::array<i32, 16>{}},
                        { "G", std::array<i32, 16>{}},
                        { "H", std::array<i32, 16>{}}
                    },
                    Unexpected{ ErrorTypeAndDescription {.Type = ETestRunErrorType::RootSignatureGeneration } }
                },
                std::tuple
                {
                    "Params contains an array",
                    "WithArray",
                    std::vector<ShaderBinding>
                    {
                    },
                    Unexpected{ ErrorTypeAndDescription {.Type = ETestRunErrorType::RootSignatureGeneration } }
                },
                std::tuple
                {
                    "Multiple constant buffers with expected values",
                    "MultipleConstantBuffers",
                    std::vector<ShaderBinding>
                    {
                        {"FirstParam", GlobalBindingsStruct{.A{2.0f}, .B{24}, .C{5.0f, 6.0f, 7.0f, 8.0f}}},
                        {"SecondParam", GlobalBindingsStruct{.A{102.5f}, .B{4195}, .C{5.0f, 10.0f, 15.0f, 28.5f}}}
                    },
                    true
                },
                std::tuple
                {
                    "Multiple constant buffers with unexpected values",
                    "MultipleConstantBuffers",
                    std::vector<ShaderBinding>
                    {
                        {"FirstParam", GlobalBindingsStruct{.A{4.0f}, .B{24}, .C{5.0f, 6.0f, 7.0f, 8.0f}}},
                        {"SecondParam", GlobalBindingsStruct{.A{102.5f}, .B{4195}, .C{5.0f, 10.0f, 15.0f, 28.5f}}}
                    },
                    false
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
            const auto results = actual.GetTestResults();
            REQUIRE(results);
            REQUIRE(!!actual == expectedResult.value() );
        }
        else
        {
            const auto results = actual.GetTestRunError();
            REQUIRE(results);
            REQUIRE(results->Type == expectedResult.error().Type);
        }
    }
}