#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

static VirtualShaderDirectoryMapping GetTestVirtualDirectoryMapping()
{
    return VirtualShaderDirectoryMapping{ "/MyTests", fs::current_path() / "shader" };
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

    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ GetTestVirtualDirectoryMapping() }
        }
    );

    auto testDesc =
        [&testName]()
        {
            return
                ShaderTestFixture::RuntimeTestDesc
            {
                .CompilationEnv
                {
                    .Source = fs::path("/MyTests/ShaderTest.hlsl")
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