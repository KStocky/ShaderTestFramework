#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

SCENARIO("MinimalShaderTestExample")
{
    stf::ShaderTestFixture fixture(stf::ShaderTestFixture::FixtureDesc{});

    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    REQUIRE(fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                // The HLSL source can either be:
                // 1. a std::filesystem::path that points to the HLSL file under test
                // 2. a std::string containing the HLSL code under test.
                .Source = std::string{
                    R"(
                        // Include the test framework
                        #include "/Test/STF/ShaderTestFramework.hlsli"

                        [numthreads(1, 1, 1)]
                        void MinimalTestEntryFunction()
                        {
                            ASSERT(AreEqual, 42, 42);
                        }
                    )"
                }
            },
            .TestName = "MinimalTestEntryFunction",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}