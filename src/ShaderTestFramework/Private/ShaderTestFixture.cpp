#include "ShaderTestFixture.h"

#include "D3D12/GPUDevice.h"

ShaderTestFixture::ShaderTestFixture()
	: m_Device(MakeUnique<GPUDevice>(GPUDevice::CreationParams
		{
			.DebugLevel = GPUDevice::EDebugLevel::DebugLayerWithValidation, 
			.DeviceType = GPUDevice::EDeviceType::Software
		}))
{
}

ShaderTestFixture::~ShaderTestFixture()
{
	// Explicitly defined to allow forward declaring of UniquePtr types
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
