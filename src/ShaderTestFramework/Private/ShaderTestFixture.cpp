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

ShaderTestFixture::ShaderTestFixture()
	: m_Device(MakeUnique<GPUDevice>(GPUDevice::CreationParams
		{
			.DebugLevel = GPUDevice::EDebugLevel::DebugLayerWithValidation, 
			.DeviceType = GPUDevice::EDeviceType::Software
		}))
{
}

bool ShaderTestFixture::IsValid() const
{
    return m_Device->IsValid();
}

bool ShaderTestFixture::IsUsingAgilitySDK() const
{
	if (!IsValid())
	{
		return false;
	}

	return m_Device->GetHardwareInfo().FeatureInfo.EnhancedBarriersSupport;
}
