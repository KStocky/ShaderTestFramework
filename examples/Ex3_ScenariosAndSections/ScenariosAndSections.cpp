#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example3Tests - Without ScenariosAndSections")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{ "/Shader", std::filesystem::current_path() / SHADER_SRC } }
        });

    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    REQUIRE(fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
                // Here we use our virtual shader directory mapping rather than the relative path
                .Source = std::filesystem::path{ "/Shader/OptionalTests.hlsl" }
            },
            .TestName = "OptionalTestsWithoutScenariosAndSections",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}

SCENARIO("Example3Tests - With ScenariosAndSections")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{ "/Shader", std::filesystem::current_path() / SHADER_SRC } }
        });

    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    REQUIRE(fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
                // Here we use our virtual shader directory mapping rather than the relative path
                .Source = std::filesystem::path{ "/Shader/OptionalTests.hlsl" }
            },
            .TestName = "OptionalTestsWithScenariosAndSections",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}

SCENARIO("Example3Tests - With ScenariosAndSections and Thread Ids")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{ "/Shader", std::filesystem::current_path() / SHADER_SRC } }
        });

    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    REQUIRE(fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
                // Here we use our virtual shader directory mapping rather than the relative path
                .Source = std::filesystem::path{ "/Shader/OptionalTests.hlsl" }
            },
            .TestName = "OptionalTestsWithScenariosAndSectionsAndThreadIds",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}