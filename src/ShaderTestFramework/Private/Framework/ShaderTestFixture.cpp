#include "Framework/ShaderTestFixture.h"

#include "D3D12/GPUDevice.h"

ShaderTestFixture::ShaderTestFixture(Desc InParams)
	: m_Device(InParams.GPUDeviceParams)
	, m_Compiler(std::move(InParams.Mappings))
{
	
}

bool ShaderTestFixture::IsValid() const
{
    return m_Device.IsValid();
}

bool ShaderTestFixture::IsUsingAgilitySDK() const
{
	if (!IsValid())
	{
		return false;
	}

	return m_Device.GetHardwareInfo().FeatureInfo.EnhancedBarriersSupport;
}
