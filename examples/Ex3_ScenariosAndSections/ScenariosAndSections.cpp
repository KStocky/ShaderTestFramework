#include <D3D12/Shader/ShaderEnums.h>
#include <Framework/ShaderTestFixture.h>

#include <string>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("Example3Tests - Without ScenariosAndSections")
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
    desc.Source = std::filesystem::path{ "/Shader/OptionalTests.hlsl" };

    ShaderTestFixture fixture(std::move(desc));

    const auto testName = GENERATE
    (
        "GIVEN_InvalidOptional_WHEN_ValidityQueried_THEN_NotValid",
        "GIVEN_InvalidOptional_WHEN_GetOrDefaultCalled_THEN_DefaultReturned",
        "GIVEN_InvalidOptional_WHEN_ValueSet_THEN_IsValid",
        "GIVEN_InvalidOptional_WHEN_ValueSet_THEN_GetReturnsValue"
    );
    
    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest(testName, 1, 1, 1));
}

SCENARIO("Example3Tests - With ScenariosAndSections")
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
    desc.Source = std::filesystem::path{ "/Shader/OptionalTests.hlsl" };

    ShaderTestFixture fixture(std::move(desc));
    
    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest("OptionalTests", 1, 1, 1));
}