#include "D3D12/Shader/IncludeHandler.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace stf
{
    namespace fs = std::filesystem;

    namespace
    {
        fs::path SanitizePath(LPCWSTR InPath)
        {
            if (InPath[0] == L'.')
            {
                InPath += 1;
            }

            if (InPath[1] == L'/')
            {
                InPath += 1;
            }

            std::wstring tempPath = InPath;

            for (auto offset = tempPath.find_first_of(L'\\'); offset != std::wstring::npos; offset = tempPath.find_first_of(L'\\', offset + 1))
            {
                tempPath[offset] = L'/';
            }

            return fs::path{ std::move(tempPath) };
        }
    }

    IncludeHandler::IncludeHandler(VirtualShaderDirectoryMappingManager InManager, ComPtr<IDxcUtils> InUtils)
        : m_DirectoryMappings(std::move(InManager))
        , m_Utils(std::move(InUtils))
        , m_RefCount(0)
    {}

    HRESULT IncludeHandler::QueryInterface(REFIID riid, void** ppvObject)
    {
        if (IsEqualIID(riid, __uuidof(IDxcIncludeHandler)))
        {
            *ppvObject = dynamic_cast<IDxcIncludeHandler*>(this);
            this->AddRef();
            return S_OK;
        }
        else if (IsEqualIID(riid, __uuidof(IUnknown)))
        {
            *ppvObject = dynamic_cast<IUnknown*>(this);
            this->AddRef();
            return S_OK;
        }
        else
        {
            return E_NOINTERFACE;
        }
    }

    ULONG IncludeHandler::AddRef()
    {
        return ++m_RefCount;
    }

    ULONG IncludeHandler::Release()
    {
        --m_RefCount;
        if (m_RefCount == 0)
        {
            delete this;
            return 0;
        }
        return m_RefCount;
    }

    HRESULT IncludeHandler::LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource)
    {

        const auto realPath = [this, path = SanitizePath(pFilename)]()
            {
                if (auto result = m_DirectoryMappings.Map(path); result.has_value())
                {
                    return result.value();
                }

                return path;
            }();

        if (!fs::exists(realPath))
        {
            return E_FAIL;
        }

        std::ifstream fileStream(realPath, std::ios::in);
        if (!fileStream.is_open())
        {
            return E_FAIL;
        }

        std::stringstream stream;
        stream << fileStream.rdbuf();

        const std::string fileContents = stream.str();

        *ppIncludeSource = nullptr;
        ComPtr<IDxcBlobEncoding> encodingBlob;
        if (const auto hr = m_Utils->CreateBlob(fileContents.c_str(), static_cast<u32>(fileContents.size()), CP_UTF8, encodingBlob.GetAddressOf()); FAILED(hr))
        {
            return hr;
        }

        const auto ret = encodingBlob->QueryInterface(IID_PPV_ARGS(ppIncludeSource));
        return ret;
    }
}
