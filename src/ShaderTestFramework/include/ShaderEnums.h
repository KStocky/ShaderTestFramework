#pragma once

#include <cstdint>

enum class EShaderCompileFlags : uint32_t
{
	Debug = (1 << 0),
	SkipValidation = (1 << 1),
	SkipOptimization = (1 << 2),
	MatrixRowMajor = (1 << 3),
	MatrixColumnMajor = (1 << 4),
	AvoidFlowControl = (1 << 9),
	PreferFlowControl = (1 << 10),
	EnableStrictness = (1 << 11),
	EnableBackwardsCompatibility = (1 << 12),
	IEEEStrictness = (1 << 13),
	O0 = (1 << 14),
	O3 = (1 << 15),
	WarningsAsErrors = (1 << 18),
	ResourcesMayAlias = (1 << 19),
	UnboundedDescriptorTables = (1 << 20),
	AllResourcesBound = (1 << 21)
};

enum class EShaderType : uint8_t
{
	None,
	Vertex,
	Hull,
	Domain,
	Geometry,
	Pixel,
	Compute
};