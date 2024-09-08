#include "D3D12/Shader/ShaderCompilerTestsCommon.h"
#include <Utility/EnumReflection.h>

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace ShaderCompilerTestsCommon;

SCENARIO("ShaderIncludeHandlerTests")
{
    using namespace stf;
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

