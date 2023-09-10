#include "ShaderTestFixture.h"

#include <dxgi1_6.h>
#include <dxgidebug.h>

#include <algorithm>
#include <cstdint>
#include <format>
#include <iostream>
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

AdapterAndFeatureLevel GetAdapter(const bool InUseSoftwareDevice, const ComPtr<IDXGIFactory7>& InFactory)
{
	if (InUseSoftwareDevice)
	{
		AdapterAndFeatureLevel ret;
		ThrowIfFailed(InFactory->EnumWarpAdapter(IID_PPV_ARGS(&ret.Adapter)));
		ret.FeatureLevel = D3D_FEATURE_LEVEL_12_1;
		return ret;
	}
	else
	{
		ComPtr<IDXGIAdapter4> adapter = nullptr;

		std::vector<AdapterAndFeatureLevel> foundAdapters;

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

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
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

	auto adapterInfo = GetAdapter(false, factory);
	ThrowIfFailed(D3D12CreateDevice(adapterInfo.Adapter.Get(), adapterInfo.FeatureLevel, IID_PPV_ARGS(m_Device.GetAddressOf())));
}

bool ShaderTestFixture::IsValid() const
{
    return m_Device.Get() != nullptr;
}