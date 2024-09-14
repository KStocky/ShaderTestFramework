#pragma once

#include "D3D12/Shader/VirtualShaderDirectoryMappingManager.h"
#include "Platform.h"
#include "Utility/Pointer.h"

#include <atomic>
#include <string>
#include <vector>

#include <Unknwn.h>
#include <dxcapi.h>

namespace stf
{
    class IncludeHandler : public IDxcIncludeHandler
    {
    public:

        struct Mapping
        {
            std::string VirtualPath;
            std::string RealPath;
        };

        explicit IncludeHandler(VirtualShaderDirectoryMappingManager InManager, ComPtr<IDxcUtils> InUtils);

        virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

        virtual ULONG STDMETHODCALLTYPE AddRef() override;

        virtual ULONG STDMETHODCALLTYPE Release() override;

        virtual HRESULT STDMETHODCALLTYPE LoadSource(LPCWSTR pFilename, IDxcBlob** ppIncludeSource) override;

    private:

        VirtualShaderDirectoryMappingManager m_DirectoryMappings;
        ComPtr<IDxcUtils> m_Utils;
        std::atomic<u32> m_RefCount;
    };
}