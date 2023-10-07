#include "ShaderTestFixture.h"

#include "D3D12AgilityDefinitions.h"

#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <format>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

static std::string HrToString(HRESULT hr)
{
    return std::format("HRESULT of {:#08x}", static_cast<uint32_t>(hr));
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) { std::cout << HrToString(hr); }
    HRESULT Error() const { return m_hr; }
private:
    const HRESULT m_hr;
};

static void ThrowIfFailed(HRESULT InHr)
{
    if (FAILED(InHr))
    {
        throw HrException(InHr);
    }
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

AdapterAndFeatureLevel GetAdapter(const bool InUseSoftwareDevice, const ComPtr<IDXGIFactory7>& InFactory)
{
	if (InUseSoftwareDevice)
	{
		std::array adapters = { ComPtr<IDXGIAdapter4>{} };
		ThrowIfFailed(InFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapters[0])));

		auto foundAdapter = GetCandidateAdapters(adapters);

		return foundAdapter[0];
	}
	else
	{
		ComPtr<IDXGIAdapter4> adapter = nullptr;

		std::vector<ComPtr<IDXGIAdapter4>> adapters;

		for (
			UINT adapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != InFactory->EnumAdapterByGpuPreference(
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


ShaderTestFixture::ShaderTestFixture()
{
    ComPtr<IDXGIFactory7> factory;
    ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(factory.GetAddressOf())));

	auto adapterInfo = GetAdapter(true, factory);
	ThrowIfFailed(D3D12CreateDevice(adapterInfo.Adapter.Get(), adapterInfo.FeatureLevel, IID_PPV_ARGS(m_Device.GetAddressOf())));
}

bool ShaderTestFixture::IsValid() const
{
    return m_Device.Get() != nullptr;
}

static bool HasFeatureLevel12_2(ID3D12Device& InDevice)
{
	D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels;
	featureLevels.NumFeatureLevels = 1;
	std::array featureLevel = { D3D_FEATURE_LEVEL_12_2 };
	featureLevels.pFeatureLevelsRequested = featureLevel.data();
	
	InDevice.CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));

	return featureLevels.MaxSupportedFeatureLevel == D3D_FEATURE_LEVEL_12_2;
}

static bool SupportsEnhancedBarriers(ID3D12Device& InDevice)
{
	D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
	bool EnhancedBarriersSupported = false;
	if (SUCCEEDED(InDevice.CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12))))
	{
		EnhancedBarriersSupported = options12.EnhancedBarriersSupported;
	}

	return EnhancedBarriersSupported;
}

bool ShaderTestFixture::IsUsingAgilitySDK() const
{
	if (!IsValid())
	{
		return false;
	}

	return SupportsEnhancedBarriers(*m_Device.Get());
}
