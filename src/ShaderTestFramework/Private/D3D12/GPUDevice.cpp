#include "D3D12/GPUDevice.h"
#include "D3D12/AgilityDefinitions.h"
#include "Utility/EnumReflection.h"
#include "Utility/Exception.h"

#include <algorithm>
#include <array>
#include <span>
#include <vector>

#include <dxgidebug.h>

namespace
{
	void SetName(ID3D12Object* InObject, const std::string_view InString)
	{
		InObject->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<uint32_t>(InString.size()), InString.data());
	}

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

	ExpectedHRes<AdapterAndFeatureLevel> GetAdapter(const GPUDevice::EDeviceType InType, IDXGIFactory7& InFactory)
	{
		if (InType == GPUDevice::EDeviceType::Software)
		{
			std::array adapters = { ComPtr<IDXGIAdapter4>{} };

			if (const auto hres = InFactory.EnumWarpAdapter(IID_PPV_ARGS(&adapters[0]));
				FAILED(hres))
			{
				return Unexpected(hres);
			}

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
	if (!SetupDebugLayer(InDesc.DebugLevel).has_value())
	{
		return;
	}
	const u32 factoryCreateFlags = InDesc.DebugLevel != EDebugLevel::Off ? DXGI_CREATE_FACTORY_DEBUG : 0;
	if (const auto hres = CreateDXGIFactory2(factoryCreateFlags, IID_PPV_ARGS(m_Factory.GetAddressOf()));
		FAILED(hres))
	{
		return;
	}

	auto adapterInfo = ThrowIfUnexpected(GetAdapter(InDesc.DeviceType, *m_Factory.Get()));

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
	
	if (!CacheHardwareInfo(m_Device.Get()).has_value())
	{
		return;
	}
}

GPUDevice::~GPUDevice()
{
	const bool wasValid = IsValid();
	m_Factory = nullptr;
	m_Adapter = nullptr;
	m_Debug = nullptr;
	m_DebugDevice = nullptr;
	const bool hasZeroRefs = m_Device.Reset() == 0;

	if (wasValid && hasZeroRefs)
	{
		// Described in https://stackoverflow.com/questions/46802508/d3d12-unavoidable-leak-report
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
}

bool GPUDevice::IsValid() const
{
	return m_Device.Get() != nullptr;
}

ExpectedHRes<CommandAllocator> GPUDevice::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE InType, std::string_view InName) const
{
	ComPtr<ID3D12CommandAllocator> allocator = nullptr;

	if (const auto hres = m_Device->CreateCommandAllocator(InType, IID_PPV_ARGS(allocator.GetAddressOf()));
		FAILED(hres))
	{
		return Unexpected{ hres };
	}
	SetName(allocator.Get(), InName);
	return CommandAllocator(CommandAllocator::CreationParams{ std::move(allocator), InType });
}

CommandList GPUDevice::CreateCommandList(D3D12_COMMAND_LIST_TYPE InType, std::string_view InName) const
{
	ComPtr<ID3D12GraphicsCommandList9> list = nullptr;

	ThrowIfFailed(m_Device->CreateCommandList1(0, InType, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(list.GetAddressOf())));
	SetName(list.Get(), InName);
	return CommandList(CommandList::CreationParams{ std::move(list) });
}

CommandQueue GPUDevice::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC& InDesc, const std::string_view InName)
{
	ComPtr<ID3D12CommandQueue> raw = nullptr;
	ThrowIfFailed(m_Device->CreateCommandQueue(&InDesc, IID_PPV_ARGS(raw.GetAddressOf())));
	SetName(raw.Get(), InName);
	return CommandQueue(CommandQueue::CreationParams{ std::move(raw), std::move(ThrowIfUnexpected(CreateFence(0ull))) });
}

ExpectedHRes<GPUResource> GPUDevice::CreateCommittedResource(const D3D12_HEAP_PROPERTIES& InHeapProps, const D3D12_HEAP_FLAGS InFlags, const D3D12_RESOURCE_DESC1& InResourceDesc, const D3D12_BARRIER_LAYOUT InInitialLayout, const D3D12_CLEAR_VALUE* InClearValue, const std::span<DXGI_FORMAT> InCastableFormats, const std::string_view InName) const
{
	ComPtr<ID3D12Resource2> raw{ nullptr };
	if (const auto hres = m_Device->CreateCommittedResource3(&InHeapProps, InFlags, &InResourceDesc, InInitialLayout, InClearValue, nullptr, static_cast<u32>(InCastableFormats.size()), InCastableFormats.data(), IID_PPV_ARGS(raw.GetAddressOf()));
		FAILED(hres))
	{
		return Unexpected(hres);
	}
	SetName(raw.Get(), InName);
	return GPUResource(GPUResource::CreationParams{ std::move(raw), InClearValue ? std::optional{*InClearValue} : std::nullopt, {D3D12_BARRIER_SYNC_NONE, D3D12_BARRIER_ACCESS_NO_ACCESS, InInitialLayout} });
}

ExpectedHRes<DescriptorHeap> GPUDevice::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC& InDesc, const std::string_view InName) const
{
	ComPtr<ID3D12DescriptorHeap> heap = nullptr;
	const auto hresult = m_Device->CreateDescriptorHeap(&InDesc, IID_PPV_ARGS(heap.GetAddressOf()));
	
	if (FAILED(hresult))
	{
		return Unexpected(hresult);
	}

	SetName(heap.Get(), InName);

	return GetDescriptorSize(InDesc.Type).transform([this, heap](const u32 InDescriptorSize) { return DescriptorHeap(DescriptorHeap::Desc{ std::move(heap), InDescriptorSize }); });
}

ExpectedHRes<Fence> GPUDevice::CreateFence(const u64 InInitialValue, const std::string_view InName) const
{
	ComPtr<ID3D12Fence1> fence = nullptr;

	if (const auto hres = m_Device->CreateFence(InInitialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
		FAILED(hres))
	{
		return Unexpected(hres);
	}
	SetName(fence.Get(), InName);
	return Fence(Fence::CreationParams{ std::move(fence), InInitialValue });
}

RootSignature GPUDevice::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC1& InDesc) const
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc;
	desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	desc.Desc_1_1 = InDesc;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	ThrowIfFailed(D3D12SerializeVersionedRootSignature(&desc, signature.GetAddressOf(), error.GetAddressOf()));

	ComPtr<ID3D12RootSignature> rootSignatureObject;
	ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignatureObject.GetAddressOf())));

	ComPtr<ID3D12VersionedRootSignatureDeserializer> deserializer;
	ThrowIfFailed(D3D12CreateVersionedRootSignatureDeserializer(signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(deserializer.GetAddressOf())));
	return RootSignature(RootSignature::CreationParams{std::move(rootSignatureObject), std::move(deserializer), std::move(signature)});
}

RootSignature GPUDevice::CreateRootSignature(const CompiledShaderData& InShader) const
{
	ComPtr<ID3D12RootSignature> rootSignatureObject;
	const auto codeBlob = InShader.GetCompiledShader();
	ThrowIfFalse(codeBlob);

	ComPtr<ID3D12VersionedRootSignatureDeserializer> deserializer;
	ThrowIfFailed(D3D12CreateVersionedRootSignatureDeserializer(codeBlob->GetBufferPointer(), codeBlob->GetBufferSize(), IID_PPV_ARGS(deserializer.GetAddressOf())));

	const auto desc = deserializer->GetUnconvertedRootSignatureDesc();

	return CreateRootSignature(desc->Desc_1_1);
}

ExpectedHRes<void> GPUDevice::SetDedicatedVideoMemoryReservation(const u64 InNewReservationBytes)
{
	if (const auto hres = m_Adapter->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, InNewReservationBytes);
		FAILED(hres))
	{
		return Unexpected(hres);
	}
	
	return {};
}

ExpectedHRes<void> GPUDevice::SetSystemVideoMemoryReservation(const u64 InNewReservationBytes)
{
	if (const auto hres = m_Adapter->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, InNewReservationBytes);
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	return {};
}

const GPUHardwareInfo& GPUDevice::GetHardwareInfo() const
{
	return *m_Info.get();
}

ExpectedHRes<void> GPUDevice::SetupDebugLayer(const EDebugLevel InDebugLevel)
{
	if (InDebugLevel == EDebugLevel::Off)
	{
		return {};
	}

	if (const auto hres = D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	m_Debug->EnableDebugLayer();
	m_Debug->SetEnableSynchronizedCommandQueueValidation(true);
	m_Debug->SetEnableAutoName(true);
	m_Debug->SetForceLegacyBarrierValidation(true);

	if (InDebugLevel == EDebugLevel::DebugLayerWithValidation)
	{
		m_Debug->SetEnableGPUBasedValidation(true);
	}

	return {};
}

ExpectedHRes<void> GPUDevice::CacheHardwareInfo(ID3D12Device12* InDevice)
{
	DXGI_ADAPTER_DESC3 adapterDesc;

	if (const auto hres = m_Adapter->GetDesc3(&adapterDesc);
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS options1{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options1, static_cast<uint32_t>(sizeof(options1)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels{};
	std::array levels{ D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_2 };
	featureLevels.NumFeatureLevels = static_cast<uint32_t>(levels.size());
	featureLevels.pFeatureLevelsRequested = levels.data();
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, static_cast<uint32_t>(sizeof(featureLevels)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	GPUVirtualAddressInfo virtualAddressInfo{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, &virtualAddressInfo, static_cast<uint32_t>(sizeof(virtualAddressInfo)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_SHADER_MODEL maxShaderModel{ .HighestShaderModel = D3D_HIGHEST_SHADER_MODEL };
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &maxShaderModel, static_cast<uint32_t>(sizeof(maxShaderModel)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS1 waveOpInfo{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &waveOpInfo, static_cast<uint32_t>(sizeof(waveOpInfo)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS2 options2{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS2, &options2, static_cast<uint32_t>(sizeof(options2)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_ROOT_SIGNATURE rootSigInfo{ .HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1 };
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &rootSigInfo, static_cast<uint32_t>(sizeof(rootSigInfo)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_ARCHITECTURE1 architectureInfo{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architectureInfo, static_cast<uint32_t>(sizeof(architectureInfo)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_SHADER_CACHE shaderCacheInfo{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_CACHE, &shaderCacheInfo, static_cast<uint32_t>(sizeof(shaderCacheInfo)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, static_cast<uint32_t>(sizeof(options5)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS6 options6{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS6, &options6, static_cast<uint32_t>(sizeof(options6)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, static_cast<uint32_t>(sizeof(options7)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12{};
	if (const auto hres = InDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, static_cast<uint32_t>(sizeof(options12)));
		FAILED(hres))
	{
		return Unexpected(hres);
	}

	m_Info = MakeShared<GPUHardwareInfo>
	(
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
	);

	return {};
}

Expected<u32, bool> GPUDevice::GetDescriptorSize(const D3D12_DESCRIPTOR_HEAP_TYPE InType) const
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
			return false;
		}
	}
}
