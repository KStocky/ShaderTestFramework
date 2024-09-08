#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"

#include <algorithm>

namespace stf
{
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

        auto FindMapping(const std::vector<VirtualShaderDirectoryMapping>& InMappings, const std::filesystem::path& InVirtualPath)
        {
            return std::ranges::find_if(InMappings,
                [&InVirtualPath](const VirtualShaderDirectoryMapping& In)
                {
                    if (InVirtualPath.native().size() <= In.VirtualPath.native().size())
                    {
                        return false;
                    }
                    const auto startsWithMapping = InVirtualPath.native().starts_with(In.VirtualPath.native());
                    const auto charAfterVirtualMap = *(InVirtualPath.native().cbegin() + In.VirtualPath.native().size());
                    const auto virtualMappingEndsWithSlash = charAfterVirtualMap == L'/' || charAfterVirtualMap == L'\\';
                    return startsWithMapping && virtualMappingEndsWithSlash;
                });
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

        if (auto foundMapping = Private::FindMapping(m_Mappings, InMapping.VirtualPath); foundMapping != m_Mappings.cend())
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

        if (auto foundMapping = Private::FindMapping(m_Mappings, InVirtualPath); foundMapping != m_Mappings.cend())
        {
            std::wstring pathString = InVirtualPath;
            const auto& realPathString = foundMapping->RealPath.native();
            const auto realPathView = std::wstring_view{ realPathString.c_str(), realPathString.ends_with(L'/') ? realPathString.size() - 1 : realPathString.size() };
            pathString.replace_with_range(pathString.cbegin(), pathString.cbegin() + foundMapping->VirtualPath.native().size(), realPathView);

            return std::filesystem::path{ std::move(pathString) };
        }

        return Unexpected{ EErrorType::NoMappingFound };
    }
}
