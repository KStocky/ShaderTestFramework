#pragma once

#include <filesystem>

namespace stf
{
    struct VirtualShaderDirectoryMapping
    {
        std::filesystem::path VirtualPath;
        std::filesystem::path RealPath;
    };
}