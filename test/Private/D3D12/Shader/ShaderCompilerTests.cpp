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

SCENARIO("HLSLTests")
{
    auto [name, successCondition] =
        GENERATE
        (
            table<std::string, bool(*)(const EHLSLVersion)>
            (
                { 
                    std::tuple
                    {
                        "FunctionStyleMacro",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "VariadicMacro",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "GlobalString",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "LocalStringAssignedToAUintArray",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "LocalString",
                        [](const EHLSLVersion) { return false; }
                    },
                     std::tuple
                    {
                        "OperatorBoolWithAnEmptyStruct",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "OperatorIntWithAnEmptyStruct",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "OperatorBool",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "OperatorInt",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "CounterMacro",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "LineMacro",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "StaticStructMemberInTemplatedStruct",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "StaticVariableInTemplatedFunction",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "StaticConstStructMemberInTemplatedStruct",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "StaticStructMemberInNonTemplatedStruct",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "MacroGeneratedStructWithStaticDataMember",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "ForwardDeclareFunction",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "ForwardDeclareFunctionInOtherFunction",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "ForwardDeclareFunctionInOtherFunctionAndDefineIt",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "DefineStructMemberFunctionLater",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "DefineStructMemberFunctionInFunction",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "CleanupAttribute",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "SizeofOperator",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "HasMemberCheck",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "ArrayTemplateSpecialization",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "NestedStructTemplates",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "TernaryInTypeTrait",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "DeferSFINAE",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "GloballyCoherentInStruct",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "GloballyCoherentTypeModifier",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "CallOperator",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    }
                }
            )
        );

    const auto hlslVersion = GENERATE(
        EHLSLVersion::v2016,
        EHLSLVersion::v2017,
        EHLSLVersion::v2018,
        EHLSLVersion::v2021
    );

    auto compiler = CreateCompiler();

    GIVEN(name)
    {
        WHEN("Compiled with HLSL version " << Enum::UnscopedName(hlslVersion))
        {
            const auto job = CreateCompilationJob(EShaderType::Compute, D3D_SHADER_MODEL_6_7, hlslVersion, {}, std::format("/Tests/HLSLTests/{}.hlsl", name));
            const auto errors = compiler.CompileShader(job);
            if (successCondition(hlslVersion))
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

SCENARIO("ShaderReflectionTests - Resource binding")
{

    auto [name, expectedNumBufferInResources] =
        GENERATE
        (
            table<std::string, u32>
            (
                {
                    std::tuple{"SingleInputBuffer", 1},
                    std::tuple{"ArrayOfBuffers", 2},
                    std::tuple{"UnboundedArrayOfBuffers", 0}
                }
            )
        );


    GIVEN(name)
    {
        WHEN("when compiled")
        {
            auto compiler = CreateCompiler();
            const auto job = CreateCompilationJob(EShaderType::Compute, D3D_SHADER_MODEL_6_0, EHLSLVersion::Default, {}, std::format("/Tests/ResourceBinding/{}.hlsl", name));
            const auto result = compiler.CompileShader(job);

            REQUIRE(result.has_value());

            AND_WHEN("Reflection is inspected")
            {
                const auto reflection = result->GetReflection();

                REQUIRE(reflection);

                D3D12_SHADER_DESC desc;
                REQUIRE(SUCCEEDED(reflection->GetDesc(&desc)));

                THEN("Has single bound resource")
                {
                    REQUIRE(desc.BoundResources == 1);

                    AND_THEN(std::format("bound resource has {} buffers", expectedNumBufferInResources))
                    {
                        D3D12_SHADER_INPUT_BIND_DESC binding;
                        REQUIRE(SUCCEEDED(reflection->GetResourceBindingDesc(0, &binding)));

                        REQUIRE(binding.BindCount == expectedNumBufferInResources);
                    }
                }

                THEN("Has zero constant buffers")
                {
                    REQUIRE(desc.ConstantBuffers == 0);
                }
            }

        }
    }
}

SCENARIO("ShaderIncludeHandlerTests")
{
    GIVEN("Shader that includes test framework")
    {
        WHEN("compiled")
        {
            auto compiler = CreateCompiler();
            const auto job = CreateCompilationJob(EShaderType::Compute, D3D_SHADER_MODEL_6_0, EHLSLVersion::v2016, {}, L"/Tests/ShaderIncludeHandlerTests/ShaderWithInclude.hlsl");
            const auto result = compiler.CompileShader(job);

            THEN("Success")
            {
                const auto error = result.has_value() ? "" : result.error();
                CAPTURE(error);
                REQUIRE(result.has_value());
            }
        }
    }
}

