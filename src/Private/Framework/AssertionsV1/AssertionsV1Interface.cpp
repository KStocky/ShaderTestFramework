
#include "Framework/AssertionsV1/AssertionsV1Interface.h"
#include "Framework/AssertionsV1/TestDataBufferProcessor.h"

#include "Utility/EnumReflection.h"

#include <format>
#include <ranges>
#include <sstream>

namespace stf::AssertionsV1
{
    namespace AssertionsV1InterfacePrivate
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

        struct PerThreadScratchData
        {
            i32 CurrentSectionID;
            i32 NextSectionID;
            i32 NextStringID;
            uint3 ThreadID;
            ScenarioSectionInfo Sections[NumSections];
        };
    }

    static D3D12_UNORDERED_ACCESS_VIEW_DESC CreateRawUAVDesc(const u32 InNumBytes)
    {
        return D3D12_UNORDERED_ACCESS_VIEW_DESC
        {
            .Format = DXGI_FORMAT_R32_TYPELESS,
            .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
            .Buffer
            {
                .FirstElement = 0,
                .NumElements = (InNumBytes + 3) / 4,
                .StructureByteStride = 0,
                .CounterOffsetInBytes = 0,
                .Flags = D3D12_BUFFER_UAV_FLAG_RAW
            }
        };
    }

    AssertionsV1Interface::AssertionsV1Interface()
    {
        using namespace AssertionsV1InterfacePrivate;

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
                        return AssertionsV1InterfacePrivate::GenericWriter<Length16>::Write;
                    }
                    case EHLSLFundamentalTypeBits::Bit32:
                    {
                        return AssertionsV1InterfacePrivate::GenericWriter<Length32>::Write;
                    }
                    case EHLSLFundamentalTypeBits::Bit64:
                    {
                        return AssertionsV1InterfacePrivate::GenericWriter<Length64>::Write;
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
                            return AssertionsV1InterfacePrivate::GenericWriter<bool>::Write;
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

    TypeReaderIndex AssertionsV1Interface::RegisterByteReader(std::string InTypeIDName, MultiTypeByteReader InByteReader)
    {
        const u32 typeId = static_cast<u32>(m_ByteReaderMap.size());
        m_ByteReaderMap.push_back(std::move(InByteReader));

        const auto defineArg = std::format(L"-D{}={}", std::wstring(InTypeIDName.begin(), InTypeIDName.end()), typeId);
        m_AdditionalArgs.push_back(defineArg);

        return TypeReaderIndex{ typeId };
    }

    TypeReaderIndex AssertionsV1Interface::RegisterByteReader(std::string InTypeIDName, SingleTypeByteReader InByteReader)
    {
        return RegisterByteReader(std::move(InTypeIDName),
            [byteReader = std::move(InByteReader)](const u16, const std::span<const std::byte> InData)
            {
                return byteReader(InData);
            }
        );
    }

    std::vector<std::wstring> AssertionsV1Interface::GetAdditionalCompilerArgs() const
    {
        return m_AdditionalArgs;
    }

    ExpectedError<AssertionsV1Interface::GPUResourcesType> AssertionsV1Interface::CreateGPUResources(ScopedCommandContext& InContext, const PerTestData& InPerTestData) const
    {
        const u32 bufferSizeInBytes = std::max(InPerTestData.GetSizeOfTestData(), 4u);
        static constexpr u32 allocationBufferSizeInBytes = sizeof(AssertionsV1::AllocationBufferData);

        const auto assertBuffer = InContext.CreateBuffer(
            GPUResourceManager::BufferDesc
            {
                .Name = "Assert data buffer",
                .RequestedSize = bufferSizeInBytes,
                .Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            }
        );

        const auto allocationBuffer = InContext.CreateBuffer(
            GPUResourceManager::BufferDesc
            {
                .Name = "Allocation data buffer",
                .RequestedSize = allocationBufferSizeInBytes,
                .Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
            }
        );

        const auto assertBufferUAV = InContext.CreateUAV(assertBuffer,
            CreateRawUAVDesc(bufferSizeInBytes));

        const auto allocationBufferUAV = InContext.CreateUAV(allocationBuffer,
            CreateRawUAVDesc(allocationBufferSizeInBytes));

        return GPUResourcesType
        {
            .AssertBuffer = assertBuffer,
            .AllocationBuffer = allocationBuffer,
            .AssertUAV = assertBufferUAV,
            .AllocationUAV = allocationBufferUAV
        };
    }

    ExpectedError<void> AssertionsV1Interface::BindShaderData(ScopedCommandShader& InShader, const GPUResourcesType& InResources, const PerTestData& InPerTestData) const
    {
        const auto dispatchDimensions = InShader.GetThreadCount();
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::DispatchDimensions", dispatchDimensions });
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::Asserts", InPerTestData.GetAssertSection() });
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::Strings", InPerTestData.GetStringSection() });
        std::ignore = InShader.StageBindingData(ShaderBinding{ "stf::AssertionsV1::detail::Sections", InPerTestData.GetSectionInfoSection() });

        std::ignore = InShader.StageBindlessResource("stf::AssertionsV1::detail::AllocationBufferIndex", InResources.AllocationUAV);
        std::ignore = InShader.StageBindlessResource("stf::AssertionsV1::detail::TestDataBufferIndex", InResources.AssertUAV);

        return {};
    }

    ExpectedError<AssertionsV1Interface::GPUReadbackResourcesType> AssertionsV1Interface::QueueReadbacks( ScopedCommandContext& InContext, const GPUResourcesType& InResources) const
    {
        return InContext.QueueReadback(InResources.AssertBuffer)
            .and_then(
                [&](const GPUResourceManager::ReadbackResultHandle InAssertReadback)
                {
                    return InContext.QueueReadback(InResources.AllocationBuffer)
                        .transform(
                            [&](const GPUResourceManager::ReadbackResultHandle InAllocationReadback)
                            {
                                return GPUReadbackResourcesType
                                {
                                    .AssertReadback = InAssertReadback,
                                    .AllocationReadback = InAllocationReadback
                                };
                            });
                });
    }

    ExpectedError<AssertionsV1Interface::TestRunResultsType> AssertionsV1Interface::ProcessReadbacks(CommandEngine& InEngine, const GPUReadbackResourcesType& InReadbacks, const PerTestData& InPerTestData) const
    {
        return InEngine.ExecuteReadback(InReadbacks.AssertReadback,
            [&](const MappedResource& InAssertData)
            {
                return InEngine.ExecuteReadback(InReadbacks.AllocationReadback,
                    [&](const MappedResource& InAllocationData) -> ExpectedError<TestRunResultsType>
                    {
                        const auto allocationData = InAllocationData.Get();
                        AssertionsV1::AllocationBufferData data;
                        std::memcpy(&data, allocationData.data(), sizeof(AssertionsV1::AllocationBufferData));
                        const auto assertData = InAssertData.Get();

                        return ProcessTestDataBuffer(data, InPerTestData, assertData, m_ByteReaderMap);
                    });
            });
    }
}
