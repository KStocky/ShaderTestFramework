#include "D3D12/Shader/ShaderCompilerTestsCommon.h"
#include <Utility/EnumReflection.h>

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace ShaderCompilerTestsCommon;

SCENARIO("ShaderModelTests")
{
    auto [name, shaderType, flags, successCondition] =
        GENERATE
        (
            table<std::string, EShaderType, std::vector<std::wstring>, bool(*)(const D3D_SHADER_MODEL)>
            (
                {
                    std::tuple
                    {
                        "ComputeShaderWith64BitIntegers",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL) { return true; }
                    },
                    std::tuple
                    {
                        "PixelShaderWithSV_Barycentrics",
                        EShaderType::Pixel,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_1; }
                    },
                    std::tuple
                    {
                        "ComputeShaderWith16BitFloats",
                        EShaderType::Compute,
                        std::vector<std::wstring>{ L"-enable-16bit-types" },
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_2; }
                    },
                    std::tuple
                    {
                        "ClosestHitShaderWithNoAttributes",
                        EShaderType::Lib,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_3 && In <= D3D_SHADER_MODEL_6_6; }
                    },
                    std::tuple
                    {
                        "ClosestHitShaderWithAttributes",
                        EShaderType::Lib,
                        std::vector<std::wstring>{L"-enable-payload-qualifiers"},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_6; }
                    },
                    std::tuple
                    {
                        "ComputeShaderWithSinglePrecisionDotAndAccumulate",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_4; }
                    },
                    std::tuple
                    {
                        "PixelShaderWithSamplerFeedback",
                        EShaderType::Pixel,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_5; }
                    },
                    std::tuple
                    {
                        "ComputeShaderWithDynamicResources",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_6; }
                    },
                    std::tuple
                    {
                        "PixelShaderWithQuadAny",
                        EShaderType::Pixel,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL) { return true; }
                    },
                    std::tuple
                    {
                        "ComputeShaderWithWritableMSAATextures",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_7; }
                    }
                }
            )
        );
    const auto shaderModel = GENERATE(
        D3D_SHADER_MODEL_6_0,
        D3D_SHADER_MODEL_6_1,
        D3D_SHADER_MODEL_6_2,
        D3D_SHADER_MODEL_6_3,
        D3D_SHADER_MODEL_6_4,
        D3D_SHADER_MODEL_6_5,
        D3D_SHADER_MODEL_6_6,
        D3D_SHADER_MODEL_6_7);

    auto compiler = CreateCompiler();

    GIVEN(name)
    {
        WHEN("Compiled with " << Enum::UnscopedName(shaderModel))
        {
            const auto job = CreateCompilationJob(shaderType, shaderModel, EHLSLVersion::Default, std::move(flags), std::format("/Tests/ShaderModelTests/{}.hlsl", name));
            const auto errors = compiler.CompileShader(job);
            if (successCondition(shaderModel))
            {
                THEN("Compilation Succeeds")
                {
                    const auto error = errors.has_value() ? "" : errors.error();
                    CAPTURE(error);
                    REQUIRE(errors.has_value());
                }
            }
            else
            {
                THEN("Compilation Fails")
                {
                    REQUIRE_FALSE(errors.has_value());
                }
            }
        }
    }
}