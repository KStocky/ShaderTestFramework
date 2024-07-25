#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example2Tests")
{
    // We can add virtual shader directory mappings to our shader test environment
    // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
    // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
    // std::filesystem::current_path() returns the current working directory
    // We set both the current working directory and the SHADER_SRC macro in our cmake script.
    ShaderTestFixture fixture(
        ShaderTestFixture::FixtureDesc
        {
            .Mappings{ VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC} }
        }
    );
    
    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    REQUIRE(fixture.RunTest(
        ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
                // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
                // Here we use our virtual shader directory mapping rather than the relative path
                .Source = std::filesystem::path{ "/Shader/MyShaderTests.hlsl" }
            },
            .TestName = "Example2Test",
            .ThreadGroupCount{ 1, 1, 1}
        })
    );
}