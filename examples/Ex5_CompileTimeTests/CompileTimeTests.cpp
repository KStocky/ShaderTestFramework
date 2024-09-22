#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example5Tests")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC} }
        });

    // RunCompileTimeTest takes a desc which is the compilation environment and the test name.
    // Compile time tests compile as a library so no entry function is required. The test name is only used to identify the test in stats
    // If the shader compiles, the test passes, if it doesn't compile, the shader fails
    // Unlike RunTest, it will not attempt to actually dispatch the shader.
    REQUIRE(fixture.RunCompileTimeTest(
        stf::ShaderTestFixture::CompileTestDesc
        {
            .CompilationEnv
            {
                // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
                // Here we use our virtual shader directory mapping rather than the relative path
                .Source = std::filesystem::path{ "/Shader/ConditionalTypeTests.hlsl" }
            },
            .TestName = "Example5Tests"
        })
    );
}