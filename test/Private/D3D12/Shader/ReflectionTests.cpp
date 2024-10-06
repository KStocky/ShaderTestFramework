#include "D3D12/Shader/ShaderCompilerTestsCommon.h"
#include <Utility/EnumReflection.h>

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace ShaderCompilerTestsCommon;

SCENARIO("ShaderReflectionTests")
{
    using namespace stf;
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
            const auto job = CreateCompilationJob(EShaderType::Compute, D3D_SHADER_MODEL_6_0, EHLSLVersion::Default, {}, std::format("/Tests/ReflectionTests/{}.hlsl", name));
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

    GIVEN("Shader with varied bindings is compiled")
    {
        auto compiler = CreateCompiler();
        const auto job = CreateCompilationJob(EShaderType::Compute, D3D_SHADER_MODEL_6_6, EHLSLVersion::v202x, {}, "/Tests/ReflectionTests/ManyTypesOfBindings.hlsl");
        const auto result = compiler.CompileShader(job);

        REQUIRE(result.has_value());

        WHEN("Reflection is inspected")
        {
            const auto reflection = result->GetReflection();

            THEN("Reflection is valid")
            {
                REQUIRE(reflection);
            }

            THEN("Has Expected Number of Resources")
            {
                D3D12_SHADER_DESC shaderDesc;
                REQUIRE(SUCCEEDED(reflection->GetDesc(&shaderDesc)));
                REQUIRE(shaderDesc.BoundResources == 5);
                REQUIRE(shaderDesc.ConstantBuffers == 4);
            }

            AND_WHEN("First constant buffer is inspected")
            {
                const auto buffer = reflection->GetConstantBufferByIndex(0);

                THEN("Has expected desc")
                {
                    D3D12_SHADER_BUFFER_DESC bufferDesc{};
                    buffer->GetDesc(&bufferDesc);

                    REQUIRE(bufferDesc.Name);
                    if (bufferDesc.Name)
                    {
                        REQUIRE(std::string_view{ bufferDesc.Name }.contains("Globals"));
                    }
                    REQUIRE(bufferDesc.Variables == 4);
                    REQUIRE(bufferDesc.Size == 32);
                    REQUIRE(bufferDesc.Type == D3D_CT_CBUFFER);
                    REQUIRE(bufferDesc.uFlags == 0);
                }

                THEN("First variable is as expected")
                {
                    auto var = buffer->GetVariableByIndex(0);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    REQUIRE(std::string_view{ varDesc.Name } == "MyNamespace::NamespacedStructBinding");
                    REQUIRE(varDesc.Size == 8u);
                    REQUIRE(varDesc.uFlags == D3D_SVF_USED);
                    REQUIRE(varDesc.StartOffset == 0);

                    auto type = var->GetType();

                    D3D12_SHADER_TYPE_DESC typeDesc{};
                    type->GetDesc(&typeDesc);

                    REQUIRE(typeDesc.Class == D3D_SVC_STRUCT);
                    REQUIRE(typeDesc.Members == 2u);
                    REQUIRE(std::string_view{ typeDesc.Name } == "MyNamespace::A");
                }

                THEN("Second variable is as expected")
                {
                    auto var = buffer->GetVariableByIndex(1);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    REQUIRE(std::string_view{ varDesc.Name } == "MyNamespace::NamespacedValueBinding");
                    REQUIRE(varDesc.Size == 4u);
                    REQUIRE(varDesc.uFlags == D3D_SVF_USED);
                    REQUIRE(varDesc.StartOffset == 8);
                }

                THEN("Third variable is as expected")
                {
                    auto var = buffer->GetVariableByIndex(2);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    REQUIRE(std::string_view{ varDesc.Name } == "ValueBinding");
                    REQUIRE(varDesc.Size == 4u);
                    REQUIRE(varDesc.uFlags == D3D_SVF_USED);
                    REQUIRE(varDesc.StartOffset == 12);
                }

                THEN("Fourth variable is as expected")
                {
                    auto var = buffer->GetVariableByIndex(3);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    REQUIRE(std::string_view{ varDesc.Name } == "NotUsed");
                    REQUIRE(varDesc.Size == 4u);
                    REQUIRE(varDesc.uFlags == 0);
                    REQUIRE(varDesc.StartOffset == 16);
                }
            }

            AND_WHEN("Second constant buffer is inspected")
            {
                const auto buffer = reflection->GetConstantBufferByIndex(1);

                THEN("Has expected desc")
                {
                    D3D12_SHADER_BUFFER_DESC bufferDesc{};
                    buffer->GetDesc(&bufferDesc);

                    if (bufferDesc.Name)
                    {
                        REQUIRE(std::string_view{ bufferDesc.Name }.contains("MyConstantBuffer"));
                    }
                    REQUIRE(bufferDesc.Variables == 1);
                    REQUIRE(bufferDesc.Size == 32);
                    REQUIRE(bufferDesc.Type == D3D_CT_CBUFFER);
                    REQUIRE(bufferDesc.uFlags == 0);
                }

                THEN("First variable is as expected")
                {
                    auto var = buffer->GetVariableByIndex(0);
                    D3D12_SHADER_VARIABLE_DESC varDesc{};
                    var->GetDesc(&varDesc);

                    REQUIRE(std::string_view{ varDesc.Name } == "MyConstantBuffer");
                    REQUIRE(varDesc.Size == 32u);
                    REQUIRE(varDesc.uFlags == D3D_SVF_USED);
                    REQUIRE(varDesc.StartOffset == 0);

                    auto type = var->GetType();

                    D3D12_SHADER_TYPE_DESC typeDesc{};
                    type->GetDesc(&typeDesc);

                    REQUIRE(typeDesc.Class == D3D_SVC_STRUCT);
                    REQUIRE(std::string_view{ typeDesc.Name } == "MyCBufferStruct");
                }
            }

            AND_WHEN("Third constant buffer is inspected")
            {
                const auto buffer = reflection->GetConstantBufferByIndex(2);

                THEN("Has expected desc")
                {
                    D3D12_SHADER_BUFFER_DESC bufferDesc{};
                    buffer->GetDesc(&bufferDesc);

                    if (bufferDesc.Name)
                    {
                        REQUIRE(std::string_view{ bufferDesc.Name }.contains("MyClassicBuffer"));
                    }
                    REQUIRE(bufferDesc.Variables == 1);
                    REQUIRE(bufferDesc.Size == 16);
                    REQUIRE(bufferDesc.Type == D3D_CT_CBUFFER);
                    REQUIRE(bufferDesc.uFlags == 0);
                }
            }

            AND_WHEN("Fourth constant buffer is inspected")
            {
                const auto buffer = reflection->GetConstantBufferByIndex(3);

                THEN("Has expected desc")
                {
                    D3D12_SHADER_BUFFER_DESC bufferDesc{};
                    buffer->GetDesc(&bufferDesc);

                    if (bufferDesc.Name)
                    {
                        REQUIRE(std::string_view{ bufferDesc.Name }.contains("MyStructBuffer"));
                    }
                    REQUIRE(bufferDesc.Variables == 1);
                    REQUIRE(bufferDesc.Size == 8);
                    REQUIRE(bufferDesc.Type == D3D_CT_RESOURCE_BIND_INFO);
                    REQUIRE(bufferDesc.uFlags == 0);
                }
            }

            AND_WHEN("First bound resource is inspected")
            {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
                reflection->GetResourceBindingDesc(0, &bindDesc);

                THEN("Has expected desc")
                {
                    REQUIRE(std::string_view{ bindDesc.Name }.contains("Globals"));
                    REQUIRE(bindDesc.Type == D3D_SIT_CBUFFER);
                    REQUIRE(bindDesc.Space == 0);
                    REQUIRE(bindDesc.BindPoint == 1);
                    REQUIRE(bindDesc.BindCount == 1);
                    REQUIRE(bindDesc.uFlags == D3D_SIF_USERPACKED);
                }
            }

            AND_WHEN("Second bound resource is inspected")
            {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
                reflection->GetResourceBindingDesc(1, &bindDesc);

                THEN("Has expected desc")
                {
                    REQUIRE(std::string_view{ bindDesc.Name } == "MyConstantBuffer");
                    REQUIRE(bindDesc.Type == D3D_SIT_CBUFFER);
                    REQUIRE(bindDesc.Space == 0);
                    REQUIRE(bindDesc.BindPoint == 0);
                    REQUIRE(bindDesc.BindCount == 1);
                    REQUIRE(bindDesc.uFlags == D3D_SIF_USERPACKED);
                }

                AND_WHEN("Buffer is inspected")
                {
                    const auto constantBuffer = reflection->GetConstantBufferByName(bindDesc.Name);

                    THEN("Buffer is valid")
                    {
                        REQUIRE(constantBuffer);
                    }

                    D3D12_SHADER_BUFFER_DESC bufferDesc;
                    constantBuffer->GetDesc(&bufferDesc);

                    THEN("Buffer is as expected")
                    {
                        REQUIRE(std::string_view{ bufferDesc.Name } == "MyConstantBuffer");
                        REQUIRE(bufferDesc.Type == D3D_CT_CBUFFER);
                        REQUIRE(bufferDesc.Variables == 1u);
                        REQUIRE(bufferDesc.Size == 32u);
                        REQUIRE(bufferDesc.uFlags == 0u);
                    }

                    THEN("Variable is as expected")
                    {
                        const auto variable = constantBuffer->GetVariableByIndex(0);
                        
                        D3D12_SHADER_VARIABLE_DESC varDesc{};
                        variable->GetDesc(&varDesc);
                        REQUIRE(std::string_view{ varDesc.Name } == "MyConstantBuffer");
                        REQUIRE(varDesc.StartOffset == 0u);
                        REQUIRE(varDesc.uFlags == D3D_SVF_USED);
                    }
                }
            }

            AND_WHEN("Third bound resource is inspected")
            {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
                reflection->GetResourceBindingDesc(2, &bindDesc);

                THEN("Has expected desc")
                {
                    REQUIRE(std::string_view{ bindDesc.Name } == "MyClassicBuffer");
                    REQUIRE(bindDesc.Type == D3D_SIT_CBUFFER);
                    REQUIRE(bindDesc.Space == 3);
                    REQUIRE(bindDesc.BindPoint == 6);
                    REQUIRE(bindDesc.BindCount == 1);
                    REQUIRE(bindDesc.uFlags == D3D_SIF_USERPACKED);
                }
            }

            AND_WHEN("Fourth bound resource is inspected")
            {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
                reflection->GetResourceBindingDesc(3, &bindDesc);

                THEN("Has expected desc")
                {
                    REQUIRE(std::string_view{ bindDesc.Name } == "MyStructBuffer");
                    REQUIRE(bindDesc.Type == D3D_SIT_STRUCTURED);
                    REQUIRE(bindDesc.Space == 0);
                    REQUIRE(bindDesc.BindPoint == 0);
                    REQUIRE(bindDesc.BindCount == 1);
                    REQUIRE(bindDesc.uFlags == 0);
                }
            }

            AND_WHEN("Fifth bound resource is inspected")
            {
                D3D12_SHADER_INPUT_BIND_DESC bindDesc{};
                reflection->GetResourceBindingDesc(4, &bindDesc);

                THEN("Has expected desc")
                {
                    REQUIRE(std::string_view{ bindDesc.Name } == "BuffArray");
                    REQUIRE(bindDesc.Type == D3D_SIT_UAV_RWTYPED);
                    REQUIRE(bindDesc.Space == 0);
                    REQUIRE(bindDesc.BindPoint == 0);
                    REQUIRE(bindDesc.BindCount == 2);
                    REQUIRE(bindDesc.uFlags == 0);
                }
            }
        }
    }
}
