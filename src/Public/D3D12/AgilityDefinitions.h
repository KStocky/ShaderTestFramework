#pragma once

#ifndef STF_D3D12_SDK_VERSION_ID
#define STF_D3D12_SDK_VERSION_ID 610
#endif

#ifndef STF_D3D12_AGILITY_SDK_PATH
#define STF_D3D12_AGILITY_SDK_PATH ".\\D3D12\\"
#endif

extern "C" { __declspec(dllexport) extern const unsigned int D3D12SDKVersion = STF_D3D12_SDK_VERSION_ID; }

extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = STF_D3D12_AGILITY_SDK_PATH; }
