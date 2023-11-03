#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"

#include <algorithm>

namespace Private
{
	static VirtualShaderDirectoryMappingManager::EErrorType ValidateVirtualPath(const std::filesystem::path& InPath)
	{
		if (InPath.empty())
		{
			return VirtualShaderDirectoryMappingManager::EErrorType::VirtualPathEmpty;
		}

		if (*InPath.begin() != L"/")
		{
			return VirtualShaderDirectoryMappingManager::EErrorType::VirtualPathShouldStartWithASlash;
		}

		return VirtualShaderDirectoryMappingManager::EErrorType::Success;
	}
}

VirtualShaderDirectoryMappingManager::EErrorType VirtualShaderDirectoryMappingManager::AddMapping(VirtualShaderDirectoryMapping InMapping)
{
	if (const auto validateResult = Private::ValidateVirtualPath(InMapping.VirtualPath); validateResult != EErrorType::Success)
	{
		return validateResult;
	}

	if (InMapping.RealPath.is_relative())
	{
		return EErrorType::RealPathMustBeAbsolute;
	}

	if (auto foundMapping = std::ranges::find_if(m_Mappings,
		[&InMapping](const VirtualShaderDirectoryMapping& In)
		{
			return In.VirtualPath.root_path() == InMapping.VirtualPath.root_path();
		}); foundMapping != m_Mappings.cend())
	{
		return EErrorType::VirtualPathAlreadyExists;
	}

	m_Mappings.push_back(std::move(InMapping));

	return EErrorType::Success;
}

VirtualShaderDirectoryMappingManager::Expected<std::filesystem::path> VirtualShaderDirectoryMappingManager::Map(const std::filesystem::path& InVirtualPath) const
{
	if (const auto validateResult = Private::ValidateVirtualPath(InVirtualPath); validateResult != EErrorType::Success)
	{
		return Unexpected{ validateResult };
	}

	if (auto foundMapping = std::ranges::find_if(m_Mappings,
		[&InVirtualPath](const VirtualShaderDirectoryMapping& In)
		{
			if (InVirtualPath.native().size() <= In.VirtualPath.native().size())
			{
				return false;
			}
			const auto startsWithMapping = InVirtualPath.native().starts_with(In.VirtualPath.native());
			const auto virtualMappingEndsWithSlash = *(InVirtualPath.native().cbegin() + In.VirtualPath.native().size()) == L'/';
			return startsWithMapping && virtualMappingEndsWithSlash;
		}); foundMapping != m_Mappings.cend())
	{
		std::wstring pathString = InVirtualPath;
		pathString.replace_with_range(pathString.cbegin(), pathString.cbegin() + foundMapping->VirtualPath.native().size(), foundMapping->RealPath.native());

		return std::filesystem::path{ std::move(pathString) };
	}

	return Unexpected{ EErrorType::NoMappingFound };
}
