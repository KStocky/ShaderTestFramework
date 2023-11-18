#pragma once

#include <filesystem>

struct VirtualShaderDirectoryMapping
{
	std::filesystem::path VirtualPath;
	std::filesystem::path RealPath;
};