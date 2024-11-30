#include <Framework/HLSLTypes.h>
#include <Framework/ShaderTestFixture.h>
#include <catch2/catch_test_macros.hpp>

SCENARIO("Example8Tests")
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

    // This struct is used in a ConstantBuffer so it must follow the packing rules
    // https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-packing-rules
    // Therefore we need a Padding member here between the float3 and int2.
    // The name does not have to match that of what is in HLSL but the memory layout does.
    // Structs used for bindings must be trivially copyable
    // i.e. No custom copy constructor, and it is valid to use std::memcpy to copy.
    struct StructUsedInHLSL
    {
        stf::float3 Vector;
        float Padding;
        stf::int2 Point;
    };

    // RunTest takes a desc that describes the test setup
    // In this case we give the HLSL source code, entry function name and thread group count.
    REQUIRE(fixture.RunTest(
        stf::ShaderTestFixture::RuntimeTestDesc
        {
            .CompilationEnv
            {
            // Instead of just giving a string that has HLSL code in it we provide a path to our HLSL source file
            // Here we use our virtual shader directory mapping rather than the relative path
                .Source = std::filesystem::path{ "/Shader/TestsUsingConstantBuffers.hlsl" }
            },
            .TestName = "TestWithConstantBuffers",
            .Bindings
            {
                // STF only supports binding globals as a whole, you can't bind individual members of a struct or cbuffer
                {"GlobalBinding", StructUsedInHLSL{.Vector{0.0, 0.0, 0.0}, .Point{42, 24}}},
                {"CBuffer", StructUsedInHLSL{.Vector{0.0, 1.0, 2.0}, .Point{0,0}}},
                // Parameters must be bound by their fully qualified name so if the binding is in a namespace it must be fully scoped.
                {"MyNamespace::GlobalParam", -1}
            },
            .ThreadGroupCount{ 1, 1, 1 }
        })
    );
}