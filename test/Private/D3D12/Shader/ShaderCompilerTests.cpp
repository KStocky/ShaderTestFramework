#include "D3D12/Shader/ShaderCompilerTestsCommon.h"
#include <Utility/EnumReflection.h>

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace ShaderCompilerTestsCommon;

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

