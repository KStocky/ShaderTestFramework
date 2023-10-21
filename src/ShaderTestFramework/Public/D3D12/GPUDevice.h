#pragma once

#include "D3D12/CommandAllocator.h"
#include "D3D12/CommandList.h"
#include "D3D12/CommandQueue.h"
#include "D3D12/Fence.h"
#include "D3D12/DescriptorHeap.h"
#include "D3D12/GPUResource.h"
#include "Platform.h"
#include "Utility/Pointer.h"

#include <span>
#include <string_view>

#include <d3d12.h>
#include <d3d12sdklayers.h>
#include <dxgi1_6.h>

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

class GPUDevice
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
	};

	GPUDevice() = default;
	GPUDevice(const CreationParams InDesc);
	~GPUDevice();

	bool IsValid() const;

	ExpectedHRes<CommandAllocator> CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE InType, std::string_view InName = "DefaultCommandAllocator") const;
	ExpectedHRes<CommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE InType, std::string_view InName = "DefaultCommandList") const;
	CommandQueue CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC& InDesc, const std::string_view InName = "DefaultCommandQueue");

	ExpectedHRes<GPUResource> CreateCommittedResource(
		const D3D12_HEAP_PROPERTIES& InHeapProps,
		const D3D12_HEAP_FLAGS InFlags,
		const D3D12_RESOURCE_DESC1& InResourceDesc,
		const D3D12_BARRIER_LAYOUT InInitialLayout,
		const D3D12_CLEAR_VALUE* InClearValue = nullptr,
		const std::span<DXGI_FORMAT> InCastableFormats = {},
		const std::string_view InName = "DefaultResource"
	) const;
	ExpectedHRes<DescriptorHeap> CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC& InDesc, const std::string_view InName = "DefaultDescriptorHeap") const;

	ExpectedHRes<Fence> CreateFence(const u64 InInitialValue, const std::string_view InName = "DefaultFence") const;

	ExpectedHRes<void> SetDedicatedVideoMemoryReservation(const u64 InNewReservationBytes);
	ExpectedHRes<void> SetSystemVideoMemoryReservation(const u64 InNewReservationBytes);

	const GPUHardwareInfo& GetHardwareInfo() const;

private:

	ExpectedHRes<void> SetupDebugLayer(const EDebugLevel InDebugLevel);
	ExpectedHRes<void> CacheHardwareInfo(ID3D12Device12* InDevice);
	Expected<u32, bool> GetDescriptorSize(const D3D12_DESCRIPTOR_HEAP_TYPE InType) const;

	ComPtr<ID3D12Device12> m_Device = nullptr;
	ComPtr<ID3D12Debug6> m_Debug = nullptr;
	ComPtr<ID3D12DebugDevice2> m_DebugDevice = nullptr;
	ComPtr<IDXGIFactory7> m_Factory = nullptr;
	ComPtr<IDXGIAdapter4> m_Adapter = nullptr;

	GPUHardwareInfo m_Info;

	u32 m_CBVDescriptorSize = 0;
	u32 m_RTVDescriptorSize = 0;
	u32 m_DSVDescriptorSize = 0;
	u32 m_SamplerDescriptorSize = 0;
};