#include "D3D12/Shader/ShaderCompilerTestsCommon.h"

#include <catch2/catch_test_macros.hpp>

SCENARIO("ShaderHashTests")
{
    GIVEN("a valid shader")
    {
        const auto shaderModel = D3D_SHADER_MODEL_6_0;

        ShaderCompilationJobDesc job;
        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = shaderModel;
        job.ShaderType = EShaderType::Compute;
        job.Source = std::string{ R"(
                        RWBuffer<int64_t> Buff;

                        int64_t InVal;
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.x] = (DispatchThreadId.x + 34) * InVal;
                        }
                        )" };
        WHEN("compiled")
        {
            ShaderCompiler compiler;
            const auto result = compiler.CompileShader(job);

            REQUIRE(result.has_value());
            
            THEN("Hash is valid")
            {
                const auto hash = result->GetShaderHash();
                REQUIRE(hash.has_value());

                AND_WHEN("is compiled again with no changes")
                {
                    const auto otherResult = compiler.CompileShader(job);
                    REQUIRE(otherResult.has_value());

                    THEN("Hash is valid")
                    {
                        const auto otherHash = otherResult->GetShaderHash();
                        REQUIRE(otherHash.has_value());

                        AND_THEN("two hashes are equal")
                        {
                            REQUIRE(*hash == *otherHash);
                        }
                    }
                }

                AND_WHEN("is compiled again with a slight change")
                {
                    job.Source = std::string{ R"(
                        RWBuffer<int64_t> Buff;

                        int64_t InVal;
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.x] = (DispatchThreadId.x + 35) * InVal;
                        }
                        )" };

                    const auto otherResult = compiler.CompileShader(job);
                    REQUIRE(otherResult.has_value());

                    THEN("Hash is valid")
                    {
                        const auto otherHash = otherResult->GetShaderHash();
                        REQUIRE(otherHash.has_value());

                        AND_THEN("two hashes are not equal")
                        {
                            REQUIRE(*hash != *otherHash);
                        }
                    }
                }
            }
        }
    }
}