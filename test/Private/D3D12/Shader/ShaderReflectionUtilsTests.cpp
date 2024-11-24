
#include "D3D12/Shader/ShaderReflectionUtils.h"
#include "D3D12/Shader/ShaderCompilerTestsCommon.h"

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace ShaderCompilerTestsCommon;

SCENARIO("IsOrContainsArray")
{
    using namespace stf;
    auto [name, succeeds] =
        GENERATE
        (
            table<std::string_view, bool>
            (
                {
                    std::tuple{"NonArrayBufferWithNoArrayMembers", false},
                    std::tuple{"NonArrayBufferStructContainsArray", true},
                    std::tuple{"NonArrayBufferStructContainsMemberThatContainsArray", true},
                    std::tuple{"ArrayBufferStruct", true}
                }
            )
        );


    GIVEN(name)
    {
        WHEN("when compiled")
        {
            auto compiler = CreateCompiler();
            const auto job = CreateCompilationJob(
                EShaderType::Compute, 
                D3D_SHADER_MODEL_6_3, 
                EHLSLVersion::Default, {}, 
                std::format("/Tests/ShaderReflectionUtilsTests/IsOrContainsArray/{}.hlsl", name)
            );
            const auto compilationResult = compiler.CompileShader(job);

            REQUIRE(compilationResult.has_value());

            AND_WHEN("Reflection inspected")
            {
                auto reflection = compilationResult->GetReflection();
                REQUIRE(reflection);

                auto constantBuffer = reflection->GetConstantBufferByName("MyBuffer");
                REQUIRE(constantBuffer);

                D3D12_SHADER_BUFFER_DESC bufferDesc;
                constantBuffer->GetDesc(&bufferDesc);

                REQUIRE(bufferDesc.Variables == 1);

                auto bufferVar = constantBuffer->GetVariableByIndex(0);
                REQUIRE(bufferVar);

                auto bufferVarType = bufferVar->GetType();
                REQUIRE(bufferVarType);

                const auto result = stf::IsOrContainsArray(*bufferVarType);
                if (succeeds)
                {
                    THEN("Contains or is array")
                    {
                        REQUIRE(result);
                    }
                }
                else
                {
                    THEN("Does not contain and is not array")
                    {
                        REQUIRE_FALSE(result);
                    }
                }
            }
        }
    }
}

SCENARIO("ConstantBufferCanBeBoundToRootConstants")
{
    using namespace stf;
    auto [name, succeeds] =
        GENERATE
        (
            table<std::string_view, bool>
            (
                {
                    std::tuple{"NonArrayBufferWithNoArrayMembers", true},
                    std::tuple{"NonArrayBufferStructContainsArray", false},
                    std::tuple{"NonArrayBufferStructContainsMemberThatContainsArray", false},
                    std::tuple{"ArrayBufferStruct", false},
                    std::tuple{"StructIsOver64UInts", false},
                }
            )
        );


    GIVEN(name)
    {
        WHEN("when compiled")
        {
            auto compiler = CreateCompiler();
            const auto job = CreateCompilationJob(
                EShaderType::Compute,
                D3D_SHADER_MODEL_6_3,
                EHLSLVersion::Default, {},
                std::format("/Tests/ShaderReflectionUtilsTests/ConstantBufferCanBeBoundToRootConstants/{}.hlsl", name)
            );
            const auto compilationResult = compiler.CompileShader(job);

            REQUIRE(compilationResult.has_value());

            AND_WHEN("Reflection inspected")
            {
                auto reflection = compilationResult->GetReflection();
                REQUIRE(reflection);

                auto constantBuffer = reflection->GetConstantBufferByName("MyBuffer");
                REQUIRE(constantBuffer);

                const auto result = stf::ConstantBufferCanBeBoundToRootConstants(*constantBuffer);
                if (succeeds)
                {
                    THEN("Can be bound to root constants")
                    {
                        REQUIRE(result);
                    }
                }
                else
                {
                    THEN("Can't be bound to root constants")
                    {
                        REQUIRE_FALSE(result);
                    }
                }
            }
        }
    }
}