#pragma once

#include "Platform.h"

enum class EShaderCompileFlags : u32
{
	None = 0,
	Debug = (1 << 0),
	SkipValidation = (1 << 1),
	SkipOptimization = (1 << 2),
	MatrixRowMajor = (1 << 3),
	MatrixColumnMajor = (1 << 4),
	AvoidFlowControl = (1 << 9),
	PreferFlowControl = (1 << 10),
	EnableStrictness = (1 << 11),
	IEEEStrictness = (1 << 13),
	O0 = (1 << 14),
	O3 = (1 << 15),
	WarningsAsErrors = (1 << 18),
	ResourcesMayAlias = (1 << 19),
	UnboundedDescriptorTables = (1 << 20),
	AllResourcesBound = (1 << 21)
};

enum class EShaderType : u8
{
	None,
	Vertex,
	Hull,
	Domain,
	Geometry,
	Pixel,
	Compute,
	Lib
};

enum class EHLSLVersion : u8
{
    v2016,
    v2017,
    v2018,
    v2021,
    v202x,
    Default = v202x
};