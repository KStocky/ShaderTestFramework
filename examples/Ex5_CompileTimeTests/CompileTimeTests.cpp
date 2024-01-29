#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example5Tests")
{
    ShaderTestFixture::Desc desc{};

    // We can add virtual shader directory mappings to our shader test environment
    // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
    // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
    // std::filesystem::current_path() returns the current working directory
    // We set both the current working directory and the SHADER_SRC macro in our cmake script.
    desc.Mappings.emplace_back(VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC});

    // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
    // Here we use our virtual shader directory mapping rather than the relative path
    desc.Source = std::filesystem::path{ "/Shader/ConditionalTypeTests.hlsl" };

    ShaderTestFixture fixture(std::move(desc));
    
    // RunCompileTimeTest takes the entry function of the shader. If the shader compiles, the test passes, if it doesn't compile, the shader fails
    // Unlike RunTest, it will not attempt to actually dispatch the shader.
    REQUIRE(fixture.RunCompileTimeTest("AllCompile"));
}