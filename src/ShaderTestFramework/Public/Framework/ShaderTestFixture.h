#pragma once

#include "D3D12/CommandEngine.h"
#include "D3D12/GPUDevice.h"
#include "D3D12/GPUResource.h"
#include "D3D12/Shader/ShaderCompiler.h"
#include "Framework/HLSLTypes.h"
#include "Framework/TestDataBufferLayout.h"
#include "Framework/TestDataBufferProcessor.h"
#include <optional>
#include <vector>

class ShaderTestFixture
{
public:

    struct Desc
    {
        std::vector<VirtualShaderDirectoryMapping> Mappings;
        GPUDevice::CreationParams GPUDeviceParams = {
            .DebugLevel = GPUDevice::EDebugLevel::DebugLayerWithValidation,
            .DeviceType = GPUDevice::EDeviceType::Software
        };
        ShaderCodeSource Source;
        std::vector<std::wstring> CompilationFlags;
        D3D_SHADER_MODEL ShaderModel = D3D_SHADER_MODEL_6_6;
        EHLSLVersion HLSLVersion = EHLSLVersion::v202x;
        STF::TestDataBufferLayout TestDataLayout{100u, 10000u, 100u, 800u, 100u};
        std::vector<ShaderMacro> Defines;
    };

    ShaderTestFixture(Desc InParams);

    void TakeCapture();
    STF::Results RunTest(const std::string_view InName, u32 InX, u32 InY, u32 InZ);
    STF::Results RunCompileTimeTest();
    void RegisterByteReader(std::string InTypeIDName, STF::MultiTypeByteReader InByteReader);
    void RegisterByteReader(std::string InTypeIDName, STF::SingleTypeByteReader InByteReader);

    bool IsValid() const;

    bool IsUsingAgilitySDK() const;

private:

    CompilationResult CompileShader(const std::string_view InName,  const EShaderType InType) const;
    CommandEngine CreateCommandEngine() const;
    DescriptorHeap CreateDescriptorHeap() const;
    PipelineState CreatePipelineState(const RootSignature& InRootSig, IDxcBlob* InShader) const;
    RootSignature CreateRootSignature(const CompiledShaderData& InShaderData) const;
    GPUResource CreateAssertBuffer(const u64 InSizeInBytes) const;
    GPUResource CreateReadbackBuffer(const u64 InSizeInBytes) const;
    DescriptorHandle CreateAssertBufferUAV(const GPUResource& InAssertBuffer, const DescriptorHeap& InHeap, const u32 InIndex) const;
    STF::Results ReadbackResults(const GPUResource& InAllocationBuffer, const GPUResource& InAssertBuffer, const uint3 InDispatchDimensions) const;
    void PopulateDefaultByteReaders();

    bool ShouldTakeCapture() const;

    GPUDevice m_Device;
    std::optional<ShaderCompiler> m_Compiler;
    ShaderCodeSource m_Source;
    STF::MultiTypeByteReaderMap m_ByteReaderMap;
    std::vector<std::wstring> m_CompilationFlags;
    std::vector<ShaderMacro> m_Defines;
    D3D_SHADER_MODEL m_ShaderModel;
    EHLSLVersion m_HLSLVersion;
    STF::TestDataBufferLayout m_TestDataLayout;
    
    u32 m_NextTypeID = 0u;
    bool m_IsWarp = false;
    bool m_CaptureRequested = false;
    bool m_PIXAvailable = false;
};
