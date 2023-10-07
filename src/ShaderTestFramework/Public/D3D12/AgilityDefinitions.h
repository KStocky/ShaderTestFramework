#pragma once
#include "Platform.h"

#ifndef D3D12_SDK_VERSION_ID
#define D3D12_SDK_VERSION_ID 610
#endif

#ifndef D3D12_AGILITY_SDK_PATH
#define D3D12_AGILITY_SDK_PATH ".\\D3D12\\"
#endif

extern "C" { __declspec(dllexport) extern const u32 D3D12SDKVersion = D3D12_SDK_VERSION_ID; }

extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = D3D12_AGILITY_SDK_PATH; }
