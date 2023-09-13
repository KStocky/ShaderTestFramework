#include <catch2/catch_test_macros.hpp>

#include "EnumReflection.h"
#include "ShaderCompiler.h"
#include "ShaderTestFixture.h"

#include <sstream>

static std::wstring ToWString(const std::string& InString)
{
	return std::wstring(InString.cbegin(), InString.cend());
}

TEST_CASE("BasicTests")
{
    SECTION("This_Should_Succeed") 
    { 
        REQUIRE(0 == 0); 
    }

    SECTION("This_Should_Also_Succeed") 
    { 
        REQUIRE(0 != 42); 
    }
}

TEST_CASE("ShaderTestFixtureTests")
{
    SECTION("This_Should_Succeed")
    {
        ShaderTestFixture Test;
        REQUIRE(Test.IsValid());
        REQUIRE(Test.IsUsingAgilitySDK());
    }

    SECTION("CompileShader_ValidSM6VertexShader_NoErrors")
    {
        ShaderCompilationJobDesc job;
        job.Source = std::string(
            R"(
            
            struct ViewCBuffer
            {
                float4x4 Projection;
                float4x4 View;
                float3 CameraPosition;
                int NumLights;
            };
            ConstantBuffer<ViewCBuffer> View : register(b0, space0);

            struct PixelIn
            {
                float4 Position : SV_Position;
                float2 TexCoord : TEXCOORD0;
            };

            PixelIn Main(uint VertexID : SV_VertexID)
            {
                PixelIn Out;
                Out.TexCoord = float2((VertexID << 1) & 2, VertexID & 2);
                Out.Position = float4(Out.TexCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
                return Out;
            }
            )");

        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = D3D_SHADER_MODEL_6_0;
        job.ShaderType = EShaderType::Vertex;
        job.Flags = Enum::MakeFlags(EShaderCompileFlags::Debug);

        const auto errors = CompileShader(job);

        std::wstringstream errorFormat;
        for (const auto error : errors)
        {
            errorFormat << ToWString(error) << L"\n";
        }

        REQUIRE(0ull == errors.size());
    }

    SECTION("CompileShader_InvalidSM6VertexShader_Errors")
    {
        ShaderCompilationJobDesc job;
        job.Source = std::string(
            R"(
            
            struct ViewCBuffer
            {
                float4x4 Projection;
                float4x4 View;
                float3 CameraPosition;
                int NumLights;
            };
            ConstantBuffer<ViewCBuffer> View : register(b0, space0);

            struct PixelIn
            {
                float4 Position : SV_Position;
                float2 TexCoord : TEXCOORD0;
            };

            PixelIn Main(uint VertexID : SV_VertexID)
            {
                PixelIn Out;
                Out.TexCoord = float2((VertexID << 1) & 2, VertexID & 2);
                Out.Position = float4(Out.TexCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
                return Out //missing semicolon
            }
            )");

        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = D3D_SHADER_MODEL_6_0;
        job.ShaderType = EShaderType::Vertex;
        job.Flags = Enum::MakeFlags(EShaderCompileFlags::Debug);

        const auto errors = CompileShader(job);

        std::wstringstream errorFormat;
        for (const auto error : errors)
        {
            errorFormat << ToWString(error) << L"\n";
        }

        REQUIRE(0ull != errors.size());
    }
}
