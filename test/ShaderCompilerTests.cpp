#include <D3D12/Shader/ShaderCompiler.h>
#include <Utility/EnumReflection.h>


#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

SCENARIO("ShaderModelTests")
{
    auto [name, code, shaderType, flags, successCondition] =
        GENERATE
        (
            table<std::string, std::string, EShaderType, std::vector<std::wstring>, bool(*)(const D3D_SHADER_MODEL)>
            (
                {
                    std::tuple
                    {
                        "Compute shader with 64 bit integers",
                        R"(
                        RWBuffer<int64_t> Buff;

                        int64_t InVal;
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.x] = (DispatchThreadId.x + 34) * InVal;
                        }
                        )",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL) { return true; }
                    },
                    std::tuple
                    {
                        "Pixel Shader with SV_Barycentrics",
                        R"(
                        float4 Main(float3 baryWeights : SV_Barycentrics) : SV_Target 
                        {
                            return float4(baryWeights, 0.0);
                        }
                        )",
                        EShaderType::Pixel,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_1; }
                    },
                    std::tuple
                    {
                        "Compute shader with 16 bit floats",
                        R"(
                        RWBuffer<float> Buff;

                        float16_t InVal;
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.x] = 32.0h * InVal;
                        }
                        )",
                        EShaderType::Compute,
                        std::vector<std::wstring>{ L"-enable-16bit-types" },
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_2; }
                    },
                    std::tuple
                    {
                        "A closest hit shader with no attributes",
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
                        )",
                        EShaderType::Lib,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_3 && In <= D3D_SHADER_MODEL_6_6; }
                    },
                    std::tuple
                    {
                        "A closest hit shader with attributes",
                        R"(
                        struct [raypayload] RayPayload
                        {
                            float Dist : read(caller) : write(closesthit);
                        };

                        [shader("closesthit")]
                        void Hit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
                        {
                            payload.Dist = RayTCurrent();
                        }
                        )",
                        EShaderType::Lib,
                        std::vector<std::wstring>{L"-enable-payload-qualifiers"},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_6; }
                    },
                    std::tuple
                    {
                        "A compute shader with single precision dot and accumulate",
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
                        )",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_4; }
                    },
                    std::tuple
                    {
                        "A pixel shader with sampler feedback",
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
                        )",
                        EShaderType::Pixel,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_5; }
                    },
                    std::tuple
                    {
                        "Compute Shader with dynamic resources",
                        R"(
                        int BuffIndex;
                        int64_t InVal;
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            RWBuffer<int64_t> Buff = ResourceDescriptorHeap[BuffIndex];
                            Buff[DispatchThreadId.x] = (DispatchThreadId.x + 34) * InVal;
                        }
                        )",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_6; }
                    },
                    std::tuple
                    {
                        "A pixel shader with QuadAny",
                        R"(
                        SamplerState s0 : register(s0);
                        Texture2D t0 : register(t0);

                        float4 Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
                        {
                            float4 ret = 0;
                            bool cond = pos.x > 500;

                            if (QuadAny(cond))
                            {
                                float4 sampled_result = float4(1.0, 2.0, 3.0, 4.0);
                                if (cond)
                                {
                                    ret = sampled_result;
                                }
                            }
                            return ret;
                        }
                        )",
                        EShaderType::Pixel,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL) { return true; }
                    },
                    std::tuple
                    {
                        "A compute shader with Writable MSAA Textures",
                        R"(
                        RWTexture2DMS<float4, 4> Buff;

                        float4 InVal;
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.xy] = InVal;
                        }
                        )",
                        EShaderType::Compute,
                        std::vector<std::wstring>{},
                        [](const D3D_SHADER_MODEL In) { return In >= D3D_SHADER_MODEL_6_7; }
                    }
                }
            )
        );

    auto shaderModel = GENERATE(
        D3D_SHADER_MODEL_6_0,
        D3D_SHADER_MODEL_6_1,
        D3D_SHADER_MODEL_6_2,
        D3D_SHADER_MODEL_6_3,
        D3D_SHADER_MODEL_6_4,
        D3D_SHADER_MODEL_6_5,
        D3D_SHADER_MODEL_6_6,
        D3D_SHADER_MODEL_6_7);

    ShaderCompilationJobDesc job;
    job.Name = "Test";
    job.EntryPoint = "Main";
    job.ShaderModel = shaderModel;
    job.ShaderType = shaderType;
    job.AdditionalFlags = flags;
    job.Source = code;

    GIVEN(name)
    {
        WHEN("Compiled with " << Enum::UnscopedName(shaderModel))
        {
            ShaderCompiler compiler;
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
                    REQUIRE(!errors.has_value());
                }
            }
        }
    }
}

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
            }
        }
    }
}

