#include <Framework/ShaderTestFixture.h>

#include <ranges>

#include <catch2/catch_test_macros.hpp>

SCENARIO("Example6Tests - Failing tests With FundamentalTypes")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC } }
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
                .Source = std::filesystem::path{ "/Shader/FailingFundamentalTypeTests.hlsl" }
            },
            .TestName = "Tests",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}

SCENARIO("Example6Tests - Failing test with no Byte Reader")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC } }
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
                .Source = std::filesystem::path{ "/Shader/FailingAssertWithoutByteReader.hlsl" }
            },
            .TestName = "Test",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}

SCENARIO("Example6Tests - Failing test with Byte Reader")
{
    stf::ShaderTestFixture fixture(
        stf::ShaderTestFixture::FixtureDesc
        {
            // We can add virtual shader directory mappings to our shader test environment
            // Here we are saying that if a file path begins with "/Shader" then it is a virtual file directory
            // and should be replaced with the path that evaluates from current_path()/SHADER_SRC
            // std::filesystem::current_path() returns the current working directory
            // We set both the current working directory and the SHADER_SRC macro in our cmake script.
            .Mappings{ stf::VirtualShaderDirectoryMapping{"/Shader", std::filesystem::current_path() / SHADER_SRC } }
        });

    // Our ByteReader takes a single parameter.
    // 1. The data of the assert.
    fixture.RegisterByteReader("MY_TYPE_READER_ID",
        [](const std::span<const std::byte> InData)
        {
            struct MyType
            {
                stf::u32 a;
                stf::f64 b;
                stf::u32 c;
            };

            MyType val;
            std::memcpy(&val, InData.data(), sizeof(MyType));

            return std::format("a = {}, b = {}, c = {}", val.a, val.b, val.c ? "true" : "false");
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
                .Source = std::filesystem::path{ "/Shader/FailingAssertWithByteReader.hlsl" }
            },
            .TestName = "Test",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}

SCENARIO("Example6Tests - Failing test with MultiType Byte Reader")
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

    // Our ByteReader takes two parameters.
    // 1. TypeId -> This is the typeid that we registered with in the shader
    // In this case it is the size of the array that we are passing back
    // But in general this can represent anything to represent how to interpret the data that is passed in.
    // 2. The data of the assert.
    fixture.RegisterByteReader("MY_TYPE_READER_ID",
        [](const stf::u16 InTypeId, const std::span<const std::byte> InData)
        {
            auto val = std::make_unique_for_overwrite<stf::u32[]>(InTypeId);
            std::memcpy(val.get(), InData.data(), InData.size_bytes());

            std::stringstream ret;
            ret << val[0];

            for (stf::u16 index = 1; index < InTypeId; ++index)
            {
                ret << ", " << val[index];
            }

            return ret.str();
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
                .Source = std::filesystem::path{ "/Shader/FailingAssertWithMultiTypeByteReader.hlsl" }
            },
            .TestName = "Test",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}

SCENARIO("Example6Tests - Failing test with Byte Writer")
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

    // Our ByteReader takes a single parameter.
    // 1. The data of the assert.
    fixture.RegisterByteReader("MY_TYPE_READER_ID",
        [](const std::span<const std::byte> InData)
        {
            if (InData.size_bytes() == 0)
            {
                return std::string{ "Empty Span!" };
            }

            const auto size = InData.size_bytes() / 4;
            auto val = std::make_unique_for_overwrite<stf::u32[]>(size);
            std::memcpy(val.get(), InData.data(), InData.size_bytes());

            std::stringstream ret;
            ret << val[0];
            for (stf::u64 index = 1; index < size; ++index)
            {
                ret << ", " << val[index];
            }

            return ret.str();
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
                .Source = std::filesystem::path{ "/Shader/FailingAssertWithByteWriter.hlsl" }
            },
            .TestName = "Test",
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}