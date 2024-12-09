#pragma once

#include "D3D12/CommandAllocator.h"
#include "D3D12/CommandList.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/Fence.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/GPUResource.h"
#include "D3D12/Shader/CompiledShaderData.h"
#include "D3D12/Shader/PipelineState.h"
#include "D3D12/Shader/RootSignature.h"
#include "Platform.h"
#include "Utility/Exception.h"
#include "Utility/Object.h"
#include "Utility/Pointer.h"

#include <span>
#include <string_view>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>
#include <d3dx12/d3dx12.h>

namespace stf
{
    template<typename T>
    using ExpectedHRes = Expected<T, HRESULT>;

    struct GPUAdapterInfo
    {
        std::wstring Name;
        uint64_t DedicatedVRAM = 0;
        uint64_t SystemRAM = 0;
        uint32_t VendorId = 0;
        uint32_t DeviceId = 0;
        uint32_t SubSysId = 0;
        uint32_t Revision = 0;
    };

    struct D3D12FeatureInfo
    {
        D3D12_SHADER_CACHE_SUPPORT_FLAGS ShaderCacheSupportFlags = D3D12_SHADER_CACHE_SUPPORT_NONE;
        D3D12_TILED_RESOURCES_TIER TiledResourceTier = D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED;
        D3D12_RESOURCE_BINDING_TIER ResourceBindingTier = D3D12_RESOURCE_BINDING_TIER_1;
        D3D12_CONSERVATIVE_RASTERIZATION_TIER ConservativeRasterizationTier = D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED;
        D3D12_RESOURCE_HEAP_TIER ResourceHeapTier = D3D12_RESOURCE_HEAP_TIER_1;
        D3D12_RENDER_PASS_TIER RenderPassTier = D3D12_RENDER_PASS_TIER_0;
        D3D12_RAYTRACING_TIER RayTracingTier = D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
        D3D_FEATURE_LEVEL MaxFeatureLevel = D3D_FEATURE_LEVEL_12_0;
        D3D_SHADER_MODEL MaxShaderModel = D3D_SHADER_MODEL_5_1;
        D3D_ROOT_SIGNATURE_VERSION RootSignatureVersion = D3D_ROOT_SIGNATURE_VERSION_1;
        D3D12_MESH_SHADER_TIER MeshShaderTier = D3D12_MESH_SHADER_TIER_NOT_SUPPORTED;
        D3D12_SAMPLER_FEEDBACK_TIER SamplerFeedbackTier = D3D12_SAMPLER_FEEDBACK_TIER_NOT_SUPPORTED;
        D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER ProgrammableSamplePositionsTier = D3D12_PROGRAMMABLE_SAMPLE_POSITIONS_TIER_NOT_SUPPORTED;
        bool DoublePrecisionSupport = false;
        bool Float10Support = false;
        bool Float16Support = false;
        bool DepthBoundsTestSupport = false;
        bool EnhancedBarriersSupport = false;
    };

    struct GPUVirtualAddressInfo
    {
        uint32_t MaxBitsPerResource = 0;
        uint32_t MaxBitsPerProcess = 0;
    };

    struct GPUWaveOperationInfo
    {
        uint32_t MinWaveLaneCount = 0;
        uint32_t MaxWaveLaneCount = 0;
        uint32_t TotalLaneCount = 0;
        bool IsSupported = false;
    };

    struct GPUArchitectureInfo
    {
        uint32_t GPUIndex = 0;
        bool SupportsTileBasedRendering = false;
        bool UMA = false;
        bool CacheCoherentUMA = false;
        bool IsolatedMMU = false;
    };

    struct VariableRateShadingInfo
    {
        uint32_t ImageTileSize = 0;
        bool AdditionalShadingRates = false;
        bool PerPrimitiveShadingRateSupportedWithViewportIndexing = false;
        bool BackgroundProcessingSupported = false;
        D3D12_VARIABLE_SHADING_RATE_TIER Tier = D3D12_VARIABLE_SHADING_RATE_TIER_NOT_SUPPORTED;
    };

    struct GPUHardwareInfo
    {
        GPUAdapterInfo AdapterInfo;
        D3D12FeatureInfo FeatureInfo;
        GPUWaveOperationInfo WaveOperationInfo;
        GPUVirtualAddressInfo VirtualAddressInfo;
        GPUArchitectureInfo ArchitectureInfo;
        VariableRateShadingInfo VRSInfo;
    };

    template<typename T>
    concept PipelineStateDescType =
        std::is_same_v<T, D3D12_GRAPHICS_PIPELINE_STATE_DESC> ||
        std::is_same_v<T, D3D12_COMPUTE_PIPELINE_STATE_DESC> ||
        std::is_same_v<T, D3DX12_MESH_SHADER_PIPELINE_STATE_DESC>;

    class GPUDevice : Object
    {
    public:

        enum class EDebugLevel : u8
        {
            Off,
            DebugLayer,
            DebugLayerWithValidation
        };

        enum class EDeviceType : u8
        {
            Hardware,
            Software
        };

        struct CreationParams
        {
            EDebugLevel DebugLevel = EDebugLevel::Off;
            EDeviceType DeviceType = EDeviceType::Software;
            bool EnableGPUCapture = false;
        };

        GPUDevice() = default;
        GPUDevice(const CreationParams InDesc);
        ~GPUDevice();

        bool IsValid() const;
        bool IsGPUCaptureEnabled() const;

        SharedPtr<CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE InType, std::string_view InName = "DefaultCommandAllocator") const;
        SharedPtr<CommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE InType, std::string_view InName = "DefaultCommandList") const;
        SharedPtr<CommandQueue> CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC& InDesc, const std::string_view InName = "DefaultCommandQueue") const;

        SharedPtr<GPUResource> CreateCommittedResource(
            const D3D12_HEAP_PROPERTIES& InHeapProps,
            const D3D12_HEAP_FLAGS InFlags,
            const D3D12_RESOURCE_DESC1& InResourceDesc,
            const D3D12_BARRIER_LAYOUT InInitialLayout,
            const D3D12_CLEAR_VALUE* InClearValue = nullptr,
            const std::span<DXGI_FORMAT> InCastableFormats = {},
            const std::string_view InName = "DefaultResource"
        ) const;

        SharedPtr<DescriptorHeap> CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC& InDesc, const std::string_view InName = "DefaultDescriptorHeap") const;

        SharedPtr<Fence> CreateFence(const u64 InInitialValue, const std::string_view InName = "DefaultFence") const;

        template<PipelineStateDescType T>
        SharedPtr<PipelineState> CreatePipelineState(const T& InDesc) const
        {
            CD3DX12_PIPELINE_STATE_STREAM5 rawStream(InDesc);
            const D3D12_PIPELINE_STATE_STREAM_DESC desc
            {
                .SizeInBytes = sizeof(rawStream),
                .pPipelineStateSubobjectStream = &rawStream
            };
            ComPtr<ID3D12PipelineState> raw = nullptr;
            ThrowIfFailed(m_Device->CreatePipelineState(&desc, IID_PPV_ARGS(raw.GetAddressOf())));
            return MakeShared<PipelineState>(PipelineState::CreationParams{ std::move(raw) });
        }

        SharedPtr<RootSignature> CreateRootSignature(const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& InDesc) const;
        SharedPtr<RootSignature> CreateRootSignature(const CompiledShaderData& InShader) const;

        void CreateShaderResourceView(const GPUResource& InResource, const DescriptorHandle InHandle) const;
        void CreateUnorderedAccessView(const GPUResource& InResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& InDesc, const DescriptorHandle InHandle) const;

        const GPUHardwareInfo& GetHardwareInfo() const;

    private:

        ExpectedHRes<void> SetupDebugLayer(const EDebugLevel InDebugLevel);
        void SetupDebugInfoQueue();
        void CacheHardwareInfo(ID3D12Device12* InDevice, IDXGIAdapter4* InAdapter);
        u32 GetDescriptorSize(const D3D12_DESCRIPTOR_HEAP_TYPE InType) const;

        ComPtr<ID3D12Device12> m_Device = nullptr;

        SharedPtr<GPUHardwareInfo> m_Info = nullptr;

        HMODULE m_PixHandle = nullptr;

        u32 m_CBVDescriptorSize = 0;
        u32 m_RTVDescriptorSize = 0;
        u32 m_DSVDescriptorSize = 0;
        u32 m_SamplerDescriptorSize = 0;
    };
}