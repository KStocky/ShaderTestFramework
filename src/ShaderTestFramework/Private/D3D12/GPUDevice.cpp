#include "D3D12/GPUDevice.h"
#include "D3D12/AgilityDefinitions.h"
#include "EnumReflection.h"
#include "Exception.h"

#include <algorithm>
#include <array>
#include <span>
#include <vector>

namespace
{
	struct AdapterAndFeatureLevel
	{
		ComPtr<IDXGIAdapter4> Adapter = nullptr;
		D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	};

	std::vector<AdapterAndFeatureLevel> GetCandidateAdapters(std::span<ComPtr<IDXGIAdapter4>> InAdapters)
	{
		std::vector<AdapterAndFeatureLevel> foundAdapters;

		for (auto adapter : InAdapters)
		{
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_2, _uuidof(ID3D12Device), nullptr)))
			{
				foundAdapters.push_back({ std::move(adapter), D3D_FEATURE_LEVEL_12_2 });
			}
			else if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
			{
				foundAdapters.push_back({ std::move(adapter), D3D_FEATURE_LEVEL_12_1 });
			}
			else if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				foundAdapters.push_back({ std::move(adapter), D3D_FEATURE_LEVEL_12_0 });
			}
			else if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_1, _uuidof(ID3D12Device), nullptr)))
			{
				foundAdapters.push_back({ std::move(adapter), D3D_FEATURE_LEVEL_11_1 });
			}
			else if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				foundAdapters.push_back({ std::move(adapter), D3D_FEATURE_LEVEL_11_0 });
			}
		}

		return foundAdapters;
	}

	AdapterAndFeatureLevel GetAdapter(const GPUDevice::EDeviceType InType, IDXGIFactory7& InFactory)
	{
		if (InType == GPUDevice::EDeviceType::Software)
		{
			std::array adapters = { ComPtr<IDXGIAdapter4>{} };
			ThrowIfFailed(InFactory.EnumWarpAdapter(IID_PPV_ARGS(&adapters[0])));

			auto foundAdapter = GetCandidateAdapters(adapters);

			return foundAdapter[0];
		}
		else
		{
			ComPtr<IDXGIAdapter4> adapter = nullptr;

			std::vector<ComPtr<IDXGIAdapter4>> adapters;

			for (
				UINT adapterIndex = 0;
				DXGI_ERROR_NOT_FOUND != InFactory.EnumAdapterByGpuPreference(
					adapterIndex,
					DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
					IID_PPV_ARGS(&adapter));
				++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					continue;
				}

				adapters.push_back(adapter);
			}

			std::vector<AdapterAndFeatureLevel> foundAdapters = GetCandidateAdapters(adapters);

			std::sort(foundAdapters.begin(), foundAdapters.end(), [](const AdapterAndFeatureLevel& InA, const AdapterAndFeatureLevel& InB)
				{
					return InA.FeatureLevel > InB.FeatureLevel;
				});

			return foundAdapters[0];
		}
	}
}

GPUDevice::GPUDevice(const CreationParams InDesc)
	: m_Device(nullptr)
	, m_Debug(nullptr)
	, m_DebugDevice(nullptr)
	, m_Factory(nullptr)
	, m_Adapter(nullptr)
	, m_CBVDescriptorSize(0)
	, m_RTVDescriptorSize(0)
	, m_DSVDescriptorSize(0)
	, m_SamplerDescriptorSize(0)
{
	SetupDebugLayer(InDesc.DebugLevel);
	const u32 factoryCreateFlags = InDesc.DebugLevel != EDebugLevel::Off ? DXGI_CREATE_FACTORY_DEBUG : 0;
	ThrowIfFailed(CreateDXGIFactory2(factoryCreateFlags, IID_PPV_ARGS(m_Factory.GetAddressOf())));

	auto adapterInfo = GetAdapter(InDesc.DeviceType, *m_Factory.Get());
	m_Adapter = std::move(adapterInfo.Adapter);
	ThrowIfFailed(D3D12CreateDevice(m_Adapter.Get(), adapterInfo.FeatureLevel, IID_PPV_ARGS(m_Device.GetAddressOf())));
	if (InDesc.DebugLevel != EDebugLevel::Off)
	{
		ThrowIfFailed(m_Device->QueryInterface(m_DebugDevice.GetAddressOf()));
	}

	m_CBVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DSVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_SamplerDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	CacheHardwareInfo();
}

GPUDevice::~GPUDevice()
{
	m_Device = nullptr;
	m_Debug = nullptr;
	if (m_DebugDevice)
	{
		// https://stackoverflow.com/questions/46802508/d3d12-unavoidable-leak-report
		// May go back to using the DXGI method of reporting live events later when I have leaks to compare the messages
		// The SO post explains things quite well
		m_DebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
	}
}

bool GPUDevice::IsValid() const
{
	return m_Device.Get() != nullptr;
}

void GPUDevice::SetDedicatedVideoMemoryReservation(const u64 InNewReservationBytes)
{
	ThrowIfFailed(m_Adapter->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, InNewReservationBytes));
}

void GPUDevice::SetSystemVideoMemoryReservation(const u64 InNewReservationBytes)
{
	ThrowIfFailed(m_Adapter->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, InNewReservationBytes));
}

const GPUHardwareInfo& GPUDevice::GetHardwareInfo() const
{
	return m_Info;
}

void GPUDevice::SetupDebugLayer(const EDebugLevel InDebugLevel)
{
	if (InDebugLevel == EDebugLevel::Off)
	{
		return;
	}

	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug)));

	m_Debug->EnableDebugLayer();
	m_Debug->SetEnableSynchronizedCommandQueueValidation(true);
	m_Debug->SetEnableAutoName(true);
	m_Debug->SetForceLegacyBarrierValidation(true);

	if (InDebugLevel == EDebugLevel::DebugLayerWithValidation)
	{
		m_Debug->SetEnableGPUBasedValidation(true);
	}
}

void GPUDevice::CacheHardwareInfo()
{
	DXGI_ADAPTER_DESC3 adapterDesc;
	ThrowIfFailed(m_Adapter->GetDesc3(&adapterDesc));

	D3D12_FEATURE_DATA_D3D12_OPTIONS options1{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options1, static_cast<uint32_t>(sizeof(options1))));

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels{};
	std::array levels{ D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_2 };
	featureLevels.NumFeatureLevels = static_cast<uint32_t>(levels.size());
	featureLevels.pFeatureLevelsRequested = levels.data();
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, static_cast<uint32_t>(sizeof(featureLevels))));

	GPUVirtualAddressInfo virtualAddressInfo{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &virtualAddressInfo, static_cast<uint32_t>(sizeof(virtualAddressInfo))));

	D3D12_FEATURE_DATA_SHADER_MODEL maxShaderModel{ .HighestShaderModel = D3D_SHADER_MODEL_6_6 };
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &maxShaderModel, static_cast<uint32_t>(sizeof(maxShaderModel))));

	D3D12_FEATURE_DATA_D3D12_OPTIONS1 waveOpInfo{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &waveOpInfo, static_cast<uint32_t>(sizeof(waveOpInfo))));

	D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, static_cast<uint32_t>(sizeof(options2))));

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSigInfo{ .HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1 };
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSigInfo, static_cast<uint32_t>(sizeof(rootSigInfo))));

	D3D12_FEATURE_DATA_ARCHITECTURE1 architectureInfo{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architectureInfo, static_cast<uint32_t>(sizeof(architectureInfo))));

	D3D12_FEATURE_DATA_SHADER_CACHE shaderCacheInfo{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCacheInfo, static_cast<uint32_t>(sizeof(shaderCacheInfo))));

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, static_cast<uint32_t>(sizeof(options5))));

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, static_cast<uint32_t>(sizeof(options6))));

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, static_cast<uint32_t>(sizeof(options7))));

	D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12{};
	ThrowIfFailed(m_Device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, static_cast<uint32_t>(sizeof(options12))));


	m_Info = GPUHardwareInfo
	{
		GPUAdapterInfo
		{
			.Name = adapterDesc.Description,
			.DedicatedVRAM = adapterDesc.DedicatedVideoMemory,
			.SystemRAM = adapterDesc.DedicatedSystemMemory,
			.VendorId = adapterDesc.VendorId,
			.DeviceId = adapterDesc.DeviceId,
			.SubSysId = adapterDesc.SubSysId,
			.Revision = adapterDesc.Revision
		},
		D3D12FeatureInfo
		{
			.ShaderCacheSupportFlags = shaderCacheInfo.SupportFlags,
			.TiledResourceTier = options1.TiledResourcesTier,
			.ResourceBindingTier = options1.ResourceBindingTier,
			.ConservativeRasterizationTier = options1.ConservativeRasterizationTier,
			.ResourceHeapTier = options1.ResourceHeapTier,
			.RenderPassTier = options5.RenderPassesTier,
			.RayTracingTier = options5.RaytracingTier,
			.MaxFeatureLevel = featureLevels.MaxSupportedFeatureLevel,
			.MaxShaderModel = maxShaderModel.HighestShaderModel,
			.RootSignatureVersion = rootSigInfo.HighestVersion,
			.MeshShaderTier = options7.MeshShaderTier,
			.SamplerFeedbackTier = options7.SamplerFeedbackTier,
			.ProgrammableSamplePositionsTier = options2.ProgrammableSamplePositionsTier,
			.DoublePrecisionSupport = !!options1.DoublePrecisionFloatShaderOps,
			.Float10Support = Enum::EnumHasMask(options1.MinPrecisionSupport, D3D12_SHADER_MIN_PRECISION_SUPPORT_10_BIT),
			.Float16Support = Enum::EnumHasMask(options1.MinPrecisionSupport, D3D12_SHADER_MIN_PRECISION_SUPPORT_16_BIT),
			.DepthBoundsTestSupport = !!options2.DepthBoundsTestSupported,
			.EnhancedBarriersSupport = !!options12.EnhancedBarriersSupported
		},
		GPUWaveOperationInfo
		{
			.MinWaveLaneCount = waveOpInfo.WaveLaneCountMin,
			.MaxWaveLaneCount = waveOpInfo.WaveLaneCountMax,
			.TotalLaneCount = waveOpInfo.TotalLaneCount,
			.IsSupported = !!waveOpInfo.WaveOps
		},
		virtualAddressInfo,
		GPUArchitectureInfo
		{
			.GPUIndex = architectureInfo.NodeIndex,
			.SupportsTileBasedRendering = !!architectureInfo.TileBasedRenderer,
			.UMA = !!architectureInfo.UMA,
			.CacheCoherentUMA = !!architectureInfo.CacheCoherentUMA,
			.IsolatedMMU = !!architectureInfo.IsolatedMMU
		},
		VariableRateShadingInfo
		{
			.ImageTileSize = options6.ShadingRateImageTileSize,
			.AdditionalShadingRates = !!options6.AdditionalShadingRatesSupported,
			.PerPrimitiveShadingRateSupportedWithViewportIndexing = !!options6.PerPrimitiveShadingRateSupportedWithViewportIndexing,
			.BackgroundProcessingSupported = !!options6.BackgroundProcessingSupported,
			.Tier = options6.VariableShadingRateTier
		}
	};
}

u32 GPUDevice::GetDescriptorSize(const D3D12_DESCRIPTOR_HEAP_TYPE InType) const
{
	switch (InType)
	{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		{
			return m_CBVDescriptorSize;
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		{
			return m_DSVDescriptorSize;
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		{
			return m_RTVDescriptorSize;
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		{
			return m_SamplerDescriptorSize;
		}
		case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
		default:
		{
			ThrowIfFalse(false, "Unsupported descriptor allocator type requested");
			return 0u;
		}
	}
}
