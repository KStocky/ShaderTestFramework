#include <Framework/ShaderTestFixture.h>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example6Tests - Failing tests With FundamentalTypes")
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
    desc.Source = std::filesystem::path{ "/Shader/FailingFundamentalTypeTests.hlsl" };

    ShaderTestFixture fixture(std::move(desc));
    
    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest("Tests", 1, 1, 1));
}

SCENARIO("Example6Tests - Failing test with no Byte Reader")
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
    desc.Source = std::filesystem::path{ "/Shader/FailingAssertWithoutByteReader.hlsl" };

    ShaderTestFixture fixture(std::move(desc));
    
    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest("Test", 1, 1, 1));
}

SCENARIO("Example6Tests - Failing test with Byte Reader")
{
    ShaderTestFixture::Desc desc{};

    // We can add virtual shader directory mappings to our shader test environment
    // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
    // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
    // std::filesystem::current_path() returns the current working directory
    // We set both the current working directory and the SHADER_SRC macro in our cmake script.
    desc.Mappings.emplace_back(VirtualShaderDirectoryMapping{ "/Shader", std::filesystem::current_path() / SHADER_SRC });

    // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
    // Here we use our virtual shader directory mapping rather than the relative path
    desc.Source = std::filesystem::path{ "/Shader/FailingAssertWithByteReader.hlsl" };

    ShaderTestFixture fixture(std::move(desc));

    fixture.RegisterByteReader("MY_TYPE_READER_ID",
        [](const std::span<const std::byte> InData)
        {
            struct MyType
            {
                u32 a;
                f64 b;
                u32 c;
            };

            MyType val;
            std::memcpy(&val, InData.data(), sizeof(MyType));

            return std::format("a = {}, b = {}, c = {}", val.a, val.b, val.c ? "true" : "false");
        });

    // RunTest takes the entry function of the shader to run and also the dispatch config.
    // In this case we are launching a single threadgroup
    REQUIRE(fixture.RunTest("Test", 1, 1, 1));
}