#include "EnumReflection.h"
#include "ShaderCompiler.h"
#include "Utility.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("ShaderModelTests")
{
    ShaderCompilationJobDesc job;
    job.Name = "Test";
    job.EntryPoint = "Main";
    job.ShaderModel = GENERATE(
        D3D_SHADER_MODEL_6_0,
        D3D_SHADER_MODEL_6_1,
        D3D_SHADER_MODEL_6_2,
        D3D_SHADER_MODEL_6_3,
        D3D_SHADER_MODEL_6_4,
        D3D_SHADER_MODEL_6_5,
        D3D_SHADER_MODEL_6_6,
        D3D_SHADER_MODEL_6_7);

    GIVEN("Compute shader with 64 bit integers")
    {
        job.Source = std::string{
            R"(
            
            RWBuffer<int64_t> Buff;

            int64_t InVal;
            [numthreads(1,1,1)]
            void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
            {
                Buff[DispatchThreadId.x] = (DispatchThreadId.x + 34) * InVal;
            }
            )" };

        job.ShaderType = EShaderType::Compute;

        WHEN("Compiled with " << Enum::UnscopedName(job.ShaderModel))
        {
            const auto errors = CompileShader(job);

            THEN("Compilation succeeds")
            {
                REQUIRE(0ull == errors.size());
            }
        }
    }

    GIVEN("Pixel Shader with SV_Barycentrics")
    {
        job.Source = std::string{
            R"(
            
            float4 Main(float3 baryWeights : SV_Barycentrics) : SV_Target 
            {
                return float4(baryWeights, 0.0);
            }
            )" };

        job.ShaderType = EShaderType::Pixel;

        WHEN("Compiled with " << Enum::UnscopedName(job.ShaderModel))
        {
            const auto errors = CompileShader(job);

            if (job.ShaderModel >= D3D_SHADER_MODEL_6_1)
            {
                THEN("Compilation succeeds")
                {
                    REQUIRE(0ull == errors.size());
                }
            }
            else
            {
                THEN("Compilation fails")
                {
                    REQUIRE(1ull == errors.size());
                }
            }
        }
    }

    GIVEN("Compute shader with 16 bit floats")
    {
        job.Source = std::string{
            R"(
            
            RWBuffer<float> Buff;

            float16_t InVal;
            [numthreads(1,1,1)]
            void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
            {
                Buff[DispatchThreadId.x] = 32.0h * InVal;
            }
            )" };

        job.ShaderType = EShaderType::Compute;
        job.AdditionalFlags.push_back(L"-enable-16bit-types");

        WHEN("Compiled with " << Enum::UnscopedName(job.ShaderModel))
        {
            const auto errors = CompileShader(job);

            if (job.ShaderModel >= D3D_SHADER_MODEL_6_2)
            {
                THEN("Compilation succeeds")
                {
                    REQUIRE(0ull == errors.size());
                }
            }
            else
            {
                THEN("Compilation fails")
                {
                    REQUIRE(1ull == errors.size());
                }
            }
        }
    }

    GIVEN("A closest hit shader with no attributes")
    {
        job.Source = std::string{
            R"(
            
            struct RayPayload
            {
                float Dist;
            };

            [shader("closesthit")]
            void Hit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
            {
                payload.Dist = RayTCurrent();
            }
            )" };

        job.ShaderType = EShaderType::Lib;

        WHEN("Compiled with " << Enum::UnscopedName(job.ShaderModel))
        {
            const auto errors = CompileShader(job);

            if (job.ShaderModel >= D3D_SHADER_MODEL_6_3 && job.ShaderModel <= D3D_SHADER_MODEL_6_6)
            {
                THEN("Compilation succeeds")
                {
                    REQUIRE(0ull == errors.size());
                }
            }
            else
            {
                THEN("Compilation fails")
                {
                    REQUIRE(1ull == errors.size());
                }
            }
        }
    }

    GIVEN("A compute shader with single precision dot and accumulate")
    {
        job.Source = std::string{
            R"(
            
            RWBuffer<uint> Buff;

            uint InVal;
            uint InVal2;
            uint InVal3;

            [numthreads(1,1,1)]
            void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
            {
                Buff[DispatchThreadId.x] = dot4add_u8packed(InVal, InVal2, InVal3);
            }
            )" };

        job.ShaderType = EShaderType::Compute;
        job.AdditionalFlags.push_back(L"-enable-16bit-types");

        WHEN("Compiled with " << Enum::UnscopedName(job.ShaderModel))
        {
            const auto errors = CompileShader(job);

            if (job.ShaderModel >= D3D_SHADER_MODEL_6_4)
            {
                THEN("Compilation succeeds")
                {
                    REQUIRE(0ull == errors.size());
                }
            }
            else
            {
                THEN("Compilation fails")
                {
                    REQUIRE(1ull == errors.size());
                }
            }
        }
    }

    GIVEN("A pixel shader with sampler feedback")
    {
        job.Source = std::string{
            R"(         

            Texture2D<float4> g_texture : register(t0);

            SamplerState g_sampler : register(s0);
            FeedbackTexture2D<SAMPLER_FEEDBACK_MIP_REGION_USED> g_feedback : register(u3);

            float4 Main() : SV_TARGET
            {
                //float2 uv = in.uv;
                g_feedback.WriteSamplerFeedback(g_texture, g_sampler, float2(0.0, 0.5));

                return g_texture.Sample(g_sampler, float2(0.5,0.5));
            }
            )" };

        job.ShaderType = EShaderType::Pixel;

        WHEN("Compiled with " << Enum::UnscopedName(job.ShaderModel))
        {
            const auto errors = CompileShader(job);

            if (job.ShaderModel >= D3D_SHADER_MODEL_6_5)
            {
                THEN("Compilation succeeds")
                {
                    REQUIRE(0ull == errors.size());
                }
            }
            else
            {
                THEN("Compilation fails")
                {
                    REQUIRE(1ull == errors.size());
                }
            }
        }
    }
}

