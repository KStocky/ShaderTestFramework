#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/AssertionsV1/ShaderTestFixture.h>

static stf::VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    return stf::VirtualShaderDirectoryMapping{ "/MyTests", stf::fs::current_path() / "shader" };
}

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("Catch2ShaderTests")
{
    auto [testName, shouldSucceed] = GENERATE
    (
        table<std::string, bool>
        (
            {
                std::tuple{"ThisTestDoesNotExist", false},
                std::tuple{"ThisTestShouldPass", true},
                std::tuple{"ThisTestShouldFail", false},
            }
        )
    );

    stf::AssertionsV1::ShaderTestFixture fixture(
        stf::AssertionsV1::ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    auto testDesc =
        [&testName]()
        {
            return
                stf::AssertionsV1::ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = std::filesystem::path("/MyTests/ShaderTest.hlsl")
                },
                .TestName = testName,
                .ThreadGroupCount{ 1, 1, 1 }
            };
        };

    DYNAMIC_SECTION(testName)
    {
        if (shouldSucceed)
        {
            REQUIRE(fixture.RunTest(testDesc()));
        }
        else
        {
            const auto result = fixture.RunTest(testDesc());
            REQUIRE_FALSE(result);
        }
    }
}