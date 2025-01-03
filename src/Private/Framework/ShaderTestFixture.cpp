#include "Framework/ShaderTestFixture.h"

#include "Framework/PIXCapturer.h"
#include "Utility/EnumReflection.h"
#include "D3D12/GPUDevice.h"

#include <format>
#include <sstream>
#include <ranges>
#include <utility>

namespace stf
{
    namespace
    {
        ShaderCompiler CreateShaderCompiler(std::vector<VirtualShaderDirectoryMapping> InMappings)
        {
            fs::path shaderDir = std::filesystem::current_path();
            shaderDir += "/";
            shaderDir += SHADER_SRC;
            InMappings.push_back({ "/Test", std::move(shaderDir) });

            return ShaderCompiler{ std::move(InMappings) };
        }
    }

    StatSystem ShaderTestFixture::statSystem;
    std::vector<TimedStat> ShaderTestFixture::cachedStats;

    ShaderTestFixture::ShaderTestFixture(FixtureDesc InParams)
        : m_Device(MakeShared<GPUDevice>(InParams.GPUDeviceParams))
        , m_TestDriver(MakeShared<ShaderTestDriver>(
            ShaderTestDriver::CreationParams
            {
                .Device = m_Device
            }
        ))
        , m_Compiler(CreateShaderCompiler(std::move(InParams.Mappings)))
        , m_Defines()
    {
        cachedStats.clear();
        PopulateDefaultByteReaders();
    }

    ShaderTestFixture::~ShaderTestFixture() noexcept
    {
        cachedStats = statSystem.FlushTimedStats();
    }

    Results ShaderTestFixture::RunTest(RuntimeTestDesc InTestDesc)
    {
        ScopedDuration fullTest(std::format("ShaderTestFixture::RunTest: {}", InTestDesc.TestName));

        InTestDesc.CompilationEnv.Defines.push_back(
            ShaderMacro
            {
                .Name = "TTL_STRING_MAX_LENGTH",
                .Definition = std::to_string(static_cast<i32>(InTestDesc.StringMaxLength))
            });

        const bool requestedRetryOnFail =
            InTestDesc.GPUCaptureMode == EGPUCaptureMode::CaptureOnFailure ||
            InTestDesc.StringMode == EStringMode::OnFailure;

        if (!requestedRetryOnFail)
        {
            return RunTestImpl(std::move(InTestDesc), false);
        }

        if (auto firstResult = RunTestImpl(InTestDesc, false))
        {
            return firstResult;
        }
        else if (auto testRunError = firstResult.GetTestRunError())
        {
            // There is no point in retrying the test if the test setup or shader compilation failed.
            return firstResult;
        }

        return RunTestImpl(std::move(InTestDesc), true);
    }

    Results ShaderTestFixture::RunCompileTimeTest(CompileTestDesc InTestDesc)
    {
        ScopedDuration scope(std::format("ShaderTestFixture::RunCompileTimeTest: {}", InTestDesc.TestName));
        return CompileShader("", EShaderType::Lib, std::move(InTestDesc.CompilationEnv), false)
            .transform(
                [](SharedPtr<ShaderTestShader>)
                {
                    return Results{ TestRunResults{} };
                })
            .or_else(
                [](ErrorTypeAndDescription InError) -> Expected<Results, std::monostate>
                {
                    return Results{ std::move(InError) };
                }
            ).value();
    }

    std::vector<TimedStat> ShaderTestFixture::GetTestStats()
    {
        return cachedStats;
    }

    Results ShaderTestFixture::RunTestImpl(RuntimeTestDesc InTestDesc, const bool InIsFailureRetry)
    {
        const bool takeCapture = ShouldTakeCapture(InTestDesc.GPUCaptureMode, InIsFailureRetry);
        const bool enableStrings = InTestDesc.StringMode == EStringMode::On || (InIsFailureRetry && InTestDesc.StringMode == EStringMode::OnFailure);
        InTestDesc.CompilationEnv.Defines.push_back(
            ShaderMacro
            {
                .Name = "TTL_ENABLE_STRINGS",
                .Definition = enableStrings ? "1" : "0"
            }
        );

        return CompileShader(InTestDesc.TestName, EShaderType::Compute, std::move(InTestDesc.CompilationEnv), takeCapture)
            .and_then(
                [](SharedPtr<ShaderTestShader> InShader) -> Expected<SharedPtr<ShaderTestShader>, ErrorTypeAndDescription>
                {
                    auto res = InShader->Init();
                    return std::move(res)
                        .transform(
                            [shader = std::move(InShader)]()
                            {
                                return std::move(shader);
                            }
                        );
                })
            .and_then(
                [this, &InTestDesc, takeCapture](SharedPtr<ShaderTestShader> InShader)
                {
                    const auto capturer = PIXCapturer(InTestDesc.TestName, takeCapture);
                    return m_TestDriver->RunShaderTest(
                        {
                            .Shader = *InShader,
                            .TestBufferLayout{ InTestDesc.TestDataLayout },
                            .Bindings = std::move(InTestDesc.Bindings),
                            .TestName = InTestDesc.TestName,
                            .DispatchConfig = InTestDesc.ThreadGroupCount
                        });
                })
            .or_else(
                [](ErrorTypeAndDescription InError) -> Expected<Results, std::monostate>
                {
                    return Results{ std::move(InError) };
                }
            ).value();
    }

    Expected<SharedPtr<ShaderTestShader>, ErrorTypeAndDescription> ShaderTestFixture::CompileShader(const std::string_view InName, const EShaderType InType, CompilationEnvDesc InCompileDesc, const bool InTakingCapture) const
    {
        ScopedDuration scope(std::format("ShaderTestFixture::CompileShader: {}", InName));
        ShaderCompilationJobDesc job;
        job.AdditionalFlags = std::move(InCompileDesc.CompilationFlags);
        job.AdditionalFlags.emplace_back(L"-enable-16bit-types");
        job.AdditionalFlags.emplace_back(L"-Wno-c++14-extensions");
        job.AdditionalFlags.emplace_back(L"-Wno-c++1z-extensions");
        job.EntryPoint = InName;
        job.ShaderModel = InCompileDesc.ShaderModel;
        job.ShaderType = InType;
        job.Source = std::move(InCompileDesc.Source);
        job.HLSLVersion = InCompileDesc.HLSLVersion;
        job.Defines = m_Defines;
        job.Defines.append_range(InCompileDesc.Defines);

        if (InTakingCapture)
        {
            job.AdditionalFlags.emplace_back(L"-Qembed_debug");
            job.AdditionalFlags.emplace_back(L"-Zss");
            job.AdditionalFlags.emplace_back(L"-Zi");
            job.Flags = Enum::MakeFlags(EShaderCompileFlags::SkipOptimization, EShaderCompileFlags::O0);
        }

        return m_Compiler
            .CompileShader(job)
            .transform(
                [this](CompiledShaderData InData)
                {
                    return MakeShared<ShaderTestShader>(ShaderTestShader::CreationParams{ .ShaderData = std::move(InData), .Device = m_Device });
                })
            .transform_error(
                [](std::string InError)
                {
                    return ErrorTypeAndDescription
                    {
                        .Type = ETestRunErrorType::ShaderCompilation,
                        .Error = std::move(InError)
                    };
                });
    }

    void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader)
    {
        const auto readerId = m_TestDriver->RegisterByteReader(InTypeIDName, std::move(InByteReader));
        m_Defines.push_back(ShaderMacro{ std::move(InTypeIDName), std::format("{}", readerId.GetIndex()) });
    }

    void ShaderTestFixture::RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader)
    {
        RegisterByteReader(std::move(InTypeIDName),
            [byteReader = std::move(InByteReader)](const u16, const std::span<const std::byte> InData)
            {
                return byteReader(InData);
            }
        );
    }

    namespace ShaderTestFixturePrivate
    {
        template<typename T>
        struct GenericWriter
        {
            static void Write(std::stringstream& InOut, const std::byte*& InBytes)
            {
                T val;
                std::memcpy(&val, InBytes, sizeof(T));
                InOut << val;
                InBytes += sizeof(T);
            }
        };

        template<>
        struct GenericWriter<bool>
        {
            static void Write(std::stringstream& InOut, const std::byte*& InBytes)
            {
                u32 val;
                std::memcpy(&val, InBytes, sizeof(u32));
                InOut << (val != 0 ? "true" : "false");
                InBytes += sizeof(u32);
            }
        };
    }

    namespace
    {
        static const int NumSections = 32;

        enum class ESectionRunState
        {
            NeverEntered,
            NeedsRun,
            Running,
            RunningEnteredSubsection,
            RunningNeedsRerun,
            Completed
        };

        struct ScenarioSectionInfo
        {
            int ParentID;
            ESectionRunState RunState;
        };

        enum class EThreadIDType
        {
            None,
            Int,
            Int3
        };

        struct ThreadIDInfo
        {
            u32 Data;
            EThreadIDType Type;
        };

        struct PerThreadScratchData
        {
            i32 CurrentSectionID;
            i32 NextSectionID;
            i32 NextStringID;
            ThreadIDInfo ThreadID;
            ScenarioSectionInfo Sections[NumSections];
        };
    }

    void ShaderTestFixture::PopulateDefaultByteReaders()
    {
        RegisterByteReader("TYPE_ID_UNDEFINED",
            [](const u16, const std::span<const std::byte> InBytes)
            {
                return std::format("Undefined Type -> {}", DefaultByteReader(0, InBytes));
            });

        RegisterByteReader("READER_ID_PER_THREAD_SCRATCH",
            [](const std::span<const std::byte> InBytes)
            {
                PerThreadScratchData data;
                std::memcpy(&data, InBytes.data(), sizeof(PerThreadScratchData));

                std::stringstream buff;
                buff << "\nCurrentSectionID: " << data.CurrentSectionID << "\n";
                buff << "NextSectionID: " << data.NextSectionID << "\n";
                buff << "NextStringID: " << data.NextStringID << "\n";
                buff << "Sections:\n--------------------------------------\n";
                for (i32 i = 0; i < NumSections; ++i)
                {
                    buff << "Section " << i << "\n";
                    buff << "ParentID: " << data.Sections[i].ParentID << "\n";
                    const auto runState = Enum::UnscopedName(data.Sections[i].RunState);
                    buff << "RunState: " << runState << "\n-------------------------\n";
                }

                return buff.str();
            });

        RegisterByteReader("READER_ID_FUNDAMENTAL",
            [](const u16 InTypeId, const std::span<const std::byte> InBytes)
            {
                enum class EHLSLFundamentalBaseType
                {
                    Bool = 0,
                    Int,
                    Uint,
                    Float
                };

                enum class EHLSLFundamentalTypeBits
                {
                    Bit16,
                    Bit32,
                    Bit64
                };
                const auto type = static_cast<EHLSLFundamentalBaseType>(InTypeId & 0x3);
                const auto numBytes = static_cast<EHLSLFundamentalTypeBits>((InTypeId >> 2) & 3);
                const u32 numColumns = ((InTypeId >> 4) & 3) + 1;
                const u32 numRows = ((InTypeId >> 6) & 3) + 1;

                const auto generateMultiLengthConcreteWriter =
                    []<typename Length16, typename Length32, typename Length64>(const EHLSLFundamentalTypeBits InNumBits)
                {
                    switch (InNumBits)
                    {
                    case EHLSLFundamentalTypeBits::Bit16:
                    {
                        return ShaderTestFixturePrivate::GenericWriter<Length16>::Write;
                    }
                    case EHLSLFundamentalTypeBits::Bit32:
                    {
                        return ShaderTestFixturePrivate::GenericWriter<Length32>::Write;
                    }
                    case EHLSLFundamentalTypeBits::Bit64:
                    {
                        return ShaderTestFixturePrivate::GenericWriter<Length64>::Write;
                    }
                    default:
                        std::unreachable();
                    }
                };


                const auto concreteWriter =
                    [generateMultiLengthConcreteWriter](const EHLSLFundamentalBaseType InType, const EHLSLFundamentalTypeBits InNumBits)
                    {
                        switch (InType)
                        {
                        case EHLSLFundamentalBaseType::Bool:
                        {
                            return ShaderTestFixturePrivate::GenericWriter<bool>::Write;
                        }
                        case EHLSLFundamentalBaseType::Float:
                        {
                            return generateMultiLengthConcreteWriter.operator() < f16, f32, f64 > (InNumBits);
                        }
                        case EHLSLFundamentalBaseType::Int:
                        {
                            return generateMultiLengthConcreteWriter.operator() < i16, i32, i64 > (InNumBits);
                        }
                        case EHLSLFundamentalBaseType::Uint:
                        {
                            return generateMultiLengthConcreteWriter.operator() < u16, u32, u64 > (InNumBits);
                        }
                        default:
                            std::unreachable();
                        }
                    }(type, numBytes);


                std::stringstream ret;
                auto bytePointer = InBytes.data();
                if (numRows > 1)
                {
                    ret << "\n";
                }
                for ([[maybe_unused]] const auto row : std::views::iota(0u, numRows))
                {
                    concreteWriter(ret, bytePointer);
                    for ([[maybe_unused]] const auto column : std::views::iota(1u, numColumns))
                    {
                        ret << ", ";
                        concreteWriter(ret, bytePointer);
                    }
                    if (row != numRows - 1)
                    {
                        ret << "\n";
                    }
                }
                return ret.str();
            });
    }

    bool ShaderTestFixture::ShouldTakeCapture(const EGPUCaptureMode InCaptureMode, const bool InIsFailureRetry) const
    {
        const bool takeCaptureIfAble = InCaptureMode == EGPUCaptureMode::On || (InIsFailureRetry && InCaptureMode == EGPUCaptureMode::CaptureOnFailure);
        return m_Device->IsGPUCaptureEnabled() && takeCaptureIfAble;
    }
}

