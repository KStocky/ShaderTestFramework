#include <D3D12/Shader/ShaderCompiler.h>
#include <Utility/EnumReflection.h>

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace
{
    ShaderCompiler CreateCompiler()
    {
        fs::path shaderDir = fs::current_path();
        shaderDir += "/";
        shaderDir += SHADER_SRC;
        shaderDir += "/D3D12/Shader/ShaderCompilerTests";

        return ShaderCompiler{ std::vector{ VirtualShaderDirectoryMapping{ "/Tests", std::move(shaderDir) } } };
    }

    ShaderCompilationJobDesc CreateCompilationJob(const EShaderType InType, const D3D_SHADER_MODEL InModel, const EHLSLVersion InVersion, std::vector<std::wstring>&& InFlags, fs::path&& InPath)
    {
        ShaderCompilationJobDesc job;
        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = InModel;
        job.ShaderType = InType;
        job.AdditionalFlags = std::move(InFlags);
        job.Source = std::move(InPath);
        job.HLSLVersion = InVersion;

        return job;
    }
}

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
                    REQUIRE(!errors.has_value());
                }
            }
        }
    }
}

SCENARIO("HLSLTests")
{
    auto [name, code, successCondition] =
        GENERATE
        (
            table<std::string, std::string, bool(*)(const EHLSLVersion)>
            (
                {
                    
                    std::tuple
                    {
                        "Function style macro",
                        R"(
                        
                        #define FUNC(InA, InB) InA + InB
                        RWBuffer<int> Buff;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.x] = FUNC(4, 3);
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Variadic macro",
                        R"(
                        
                        #define ENTRY(InName, ...) void InName(__VA_ARGS__)

                        [numthreads(1,1,1)]
                        ENTRY(Main, uint3 DispatchThreadId : SV_DispatchThreadID, uint GroupIndex : SV_GroupIndex)
                        {
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Global string",
                        R"(
                        string hello = "Hi";
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Local string assigned to a uint array",
                        R"(
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            uint hello[] = "Hi";
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "Local string",
                        R"(
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            string hello = "Hi";
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                     std::tuple
                    {
                        "Operator bool with an empty struct",
                        R"(
                        
                        struct MyStruct
                        {
                            operator bool()
                            {
                                return true;
                            }
                        };
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            MyStruct testStruct;
                            bool t = (bool)testStruct;
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "Operator int with an empty struct",
                        R"(
                        
                        struct MyStruct
                        {
                            operator int()
                            {
                                return 0;
                            }
                        };
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            MyStruct testStruct;
                            int u = (int)testStruct;
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "Operator bool",
                        R"(
                        
                        struct MyStruct
                        {
                            int hi;
                            operator bool()
                            {
                                return true;
                            }
                        };
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            MyStruct testStruct;
                            bool t = (bool)testStruct;
                        }
                        )",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "Operator int",
                        R"(
                        
                        struct MyStruct
                        {
                            int hi;
                            
                            operator int()
                            {
                                return 0;
                            }
                        };
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            MyStruct testStruct;
                            int u = (int)testStruct;
                        }
                        )",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "Counter macro",
                        R"(
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            int a = __COUNTER__;
                            int b = __COUNTER__;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Line macro",
                        R"(
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            int a = __LINE__;
                            int b = __LINE__;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "static struct member in templated struct",
                        R"(
                        template<int ID>
                        struct Test
                        {
                            static int Num;
                        };
                        
                        template<int ID>
                        int Test<ID>::Num = 2;
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           int i = Test<0>::Num;
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "static variable in templated function",
                        R"(
                        template<int ID>
                        int StaticFunc(int In, bool InShouldChange)
                        {
                            static int var = 0;
                            if (InShouldChange)
                            {
                                var = In;
                            }
                            return var;
                        }
                        
                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           int i = StaticFunc<0>(5, true);
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                        "static const struct member in templated struct",
                        R"(
                        template<int ID>
                        struct Test
                        {
                            static const int Num = 2;
                        };

                        RWBuffer<int> MyBuffer;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           MyBuffer[DispatchThreadId.x] = Test<42>::Num;
                        }
                        )",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; }
                    },
                    std::tuple
                    {
                        "static struct member in non templated struct",
                        R"(
                        struct Test
                        {
                            static int Num;
                        };

                        int Test::Num = 2;
                        RWBuffer<int> MyBuffer;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           MyBuffer[DispatchThreadId.x] = Test::Num;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Macro generated struct with static data member",
                        R"(
                        struct Test
                        {
                            static int Num;
                        };

                        int Test::Num = 2;
                        RWBuffer<int> MyBuffer;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           MyBuffer[DispatchThreadId.x] = Test::Num;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Forward Declare function",
                        R"(
                        
                        int GetAnswer();
                        RWBuffer<int> MyBuffer;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           MyBuffer[DispatchThreadId.x] = GetAnswer();
                        }

                        int GetAnswer()
                        {
                            return 42;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                        "Forward Declare function in other function",
                        R"(
                        
                        int GetAnswer();
                        RWBuffer<int> MyBuffer;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           int GetAnswer();
                           MyBuffer[DispatchThreadId.x] = GetAnswer();
                        }

                        int GetAnswer()
                        {
                            return 42;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                    "Forward Declare function in other function and define it",
                    R"(
                        
                        int GetAnswer();
                        RWBuffer<int> MyBuffer;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                           int GetAnswer();
                           MyBuffer[DispatchThreadId.x] = GetAnswer();

                            int GetAnswer()
                            {
                                return 42;
                            }
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                    "Define struct member function later",
                    R"(
                        
                        RWBuffer<int> MyBuffer;

                        struct TestStruct
                        {
                            int GetAnswer();
                        };

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            TestStruct t;
                            MyBuffer[DispatchThreadId.x] = t.GetAnswer();
                        }
                        
                        int TestStruct::GetAnswer()
                        {
                            return 42;
                        }
                        )",
                        [](const EHLSLVersion) { return true; }
                    },
                    std::tuple
                    {
                    "Define struct member function in function",
                    R"(
                        
                        RWBuffer<int> MyBuffer;

                        struct TestStruct
                        {
                            int GetAnswer();
                        };

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            TestStruct t;
                            MyBuffer[DispatchThreadId.x] = t.GetAnswer();

                            int TestStruct::GetAnswer()
                            {
                                return 42;
                            }
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
                    },
                    std::tuple
                    {
                    "Cleanup attribute",
                    R"(
                        
                        RWBuffer<int> MyBuffer;
                        void DoTheThing(){}

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            int x __attribute__((cleanup(DoTheThing)));
                            MyBuffer[DispatchThreadId.x] = 4;
                        }
                        )",
                        [](const EHLSLVersion) { return false; }
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

    ShaderCompilationJobDesc job;
    job.Name = "Test";
    job.EntryPoint = "Main";
    job.ShaderModel = D3D_SHADER_MODEL_6_7;
    job.ShaderType = EShaderType::Compute;
    job.Source = code;
    job.HLSLVersion = hlslVersion;

    GIVEN(name)
    {
        WHEN("Compiled with HLSL version " << Enum::UnscopedName(hlslVersion))
        {
            ShaderCompiler compiler;
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

SCENARIO("ShaderReflectionTests - Resource binding")
{

    auto [name, code, expectedNumBufferInResources] =
        GENERATE
        (
            table<std::string, std::string, u32>
            (
                {
                    std::tuple
                    {
                        "A shader with a single input Buffer",
                        R"(
                        RWBuffer<int> Buff;

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[DispatchThreadId.x] = 0;
                        }
                        )",
                        1
                    },
                    std::tuple
                    {
                        "A shader with an array of Buffers",
                        R"(
                        RWBuffer<int> Buff[2];

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[0][DispatchThreadId.x] = 0;
                            Buff[1][DispatchThreadId.x] = 0;
                        }
                        )",
                        2
                    },
                    std::tuple
                    {
                        "A shader with an unbounded array of Buffers",
                        R"(
                        RWBuffer<int> Buff[];

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            Buff[0][DispatchThreadId.x] = 0;
                            Buff[1][DispatchThreadId.x] = 0;
                        }
                        )",
                        0
                    }
                }
            )
        );


    GIVEN(name)
    {
        ShaderCompilationJobDesc job;
        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = D3D_SHADER_MODEL_6_0;
        job.ShaderType = EShaderType::Compute;
        job.Source = code;

        WHEN("when compiled")
        {
            ShaderCompiler compiler;
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
        ShaderCompilationJobDesc job;
        job.Name = "Test";
        job.EntryPoint = "Main";
        job.ShaderModel = D3D_SHADER_MODEL_6_6;
        job.ShaderType = EShaderType::Compute;
        job.HLSLVersion = EHLSLVersion::v2021;
        job.Source = std::string{ R"(
                        #include "/Test/Public/ShaderTestFramework.hlsli"

                        [numthreads(1,1,1)]
                        void Main(uint3 DispatchThreadId : SV_DispatchThreadID)
                        {
                            ShaderTestPrivate::Success();
                        }
                        )" };

        WHEN("compiled")
        {
            ShaderCompiler compiler;
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

