#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

SCENARIO("MinimalShaderTestExample")
{
    ShaderTestFixture::Desc desc{};

    desc.HLSLVersion = EHLSLVersion::v2021;

    // The HLSL source can either be:
    // 1. a std::filesystem::path that points to the HLSL file under test
    // 2. a std::string containing the HLSL code under test.
    desc.Source = std::string{
        R"(
            // Include the test framework
            #include "/Test/STF/ShaderTestFramework.hlsli"

            [RootSignature(SHADER_TEST_RS)]
            [numthreads(1, 1, 1)]
            void MinimalTestEntryFunction()
            {
                STF::AreEqual(42, 42);
            }
        )"
    };
    ShaderTestFixture fixture(std::move(desc));

    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest("MinimalTestEntryFunction", 1, 1, 1));
}