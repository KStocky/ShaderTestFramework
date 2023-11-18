#include "D3D12/Shader/IncludeHandler.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>

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
    namespace fs = std::filesystem;

    if ((pFilename[0] == L'.') && (pFilename[1] == L'/'))
    {
        pFilename += 2;
    }
    
    const auto realPath = [this, pFilename]()
    {
        std::wstring fileName = pFilename;

        if (auto result = m_DirectoryMappings.Map(std::filesystem::path{ fileName }); result.has_value())
        {
            return result.value();
        }

        return fs::path{ fileName };
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
