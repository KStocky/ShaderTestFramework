#pragma once

#include "D3D12/Shader/VirtualShaderDirectoryMapping.h"
#include "Utility/Expected.h"

class VirtualShaderDirectoryMappingManager
{
public:

	enum class EErrorType
	{
		Success,
		NoMappingFound,
		RealPathMustBeAbsolute,
		VirtualPathAlreadyExists,
		VirtualPathEmpty,
		VirtualPathShouldStartWithASlash
	};

	template<typename T>
	using Expected = Expected<T, EErrorType>;

	VirtualShaderDirectoryMappingManager() = default;

	EErrorType AddMapping(VirtualShaderDirectoryMapping InMapping);
	Expected<std::filesystem::path> Map(const std::filesystem::path& InVirtualPath) const;


private:

	std::vector<VirtualShaderDirectoryMapping> m_Mappings;
};