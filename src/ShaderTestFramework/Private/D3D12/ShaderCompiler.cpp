#include "ShaderCompiler.h"

#include "EnumReflection.h"
#include "Pointer.h"
#include "Utility.h"

#include <fstream>

#include <dxcapi.h>

ShaderCodeSource::ShaderCodeSource(std::string InSourceCode)
	: m_Source(std::move(InSourceCode))
{}

ShaderCodeSource::ShaderCodeSource(fs::path InSourcePath)
	: m_Source(std::move(InSourcePath))
{}

ShaderCodeSource::operator std::string() const
{
	return ToString();
}

std::string ShaderCodeSource::ToString() const
{
	return std::visit(OverloadSet{
		[](std::monostate)
		{
			return std::string();
		},
		[](const std::string& InSource)
		{
			return InSource;
		},
		[](const fs::path& InPath)
		{
			std::ifstream file(InPath);

			if (file.is_open())
			{
				std::stringstream ret;
				ret << file.rdbuf();
				return ret.str();
			}
			else
			{
				return std::string();
			}
		} }, m_Source);
}

std::string MakeShaderTarget(const D3D_SHADER_MODEL InShaderModel, const EShaderType InType)
{
	const auto typeName = Enum::UnscopedName(InType);
	const auto modelName = Enum::UnscopedName(InShaderModel);

	std::string ret;
	if (InType != EShaderType::Lib)
	{
		ret += static_cast<char>(std::tolower(typeName[0]));
		ret += "s_";
	}
	else
	{
		ret += "lib_";
	}
	ret += modelName.substr(modelName.rfind("L_") + 2);

	return ret;
}

std::vector<std::string> CompileShaderDXC(const ShaderCompilationJobDesc& InJob)
{
	std::vector<std::string> errors;

	const auto source = InJob.Source.ToString();

	ComPtr<IDxcUtils> utils;
	ComPtr<IDxcCompiler3> compiler;
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

	ComPtr<IDxcIncludeHandler> includeHandler;
	utils->CreateDefaultIncludeHandler(&includeHandler);

	DxcBuffer sourceBuffer;
	sourceBuffer.Encoding = DXC_CP_ACP;
	sourceBuffer.Ptr = source.c_str();
	sourceBuffer.Size = source.size();

	std::vector<std::wstring> args;

	if (InJob.Name.size() > 0ull)
	{
		args.push_back(ToWString(InJob.Name));
	}

	args.push_back(L"-E");
	args.push_back(ToWString(InJob.EntryPoint));
	args.push_back(L"-T");
	args.push_back(ToWString(MakeShaderTarget(InJob.ShaderModel, InJob.ShaderType)));

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::AllResourcesBound))
	{
		args.push_back(L"-all-resources-bound");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::AvoidFlowControl))
	{
		args.push_back(L"-GFa");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::Debug))
	{
		args.push_back(L"-Zi");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::EnableBackwardsCompatibility))
	{
		args.push_back(L"-Gec");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::EnableStrictness))
	{
		args.push_back(L"-Ges");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::IEEEStrictness))
	{
		args.push_back(L"-Gis");
	}

	static constexpr auto matrixFlags = Enum::MakeFlags(EShaderCompileFlags::MatrixColumnMajor, EShaderCompileFlags::MatrixRowMajor);
	const auto NoMatrixPackingPreference = Enum::EnumHasMaskNotSet(InJob.Flags, matrixFlags);
	
	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::MatrixRowMajor) || 
		NoMatrixPackingPreference)
	{
		args.push_back(L"-Zpr");
	}
	else
	{
		args.push_back(L"-Zpc");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::O0))
	{
		args.push_back(L"-O0");
	}
	else
	{
		args.push_back(L"-O3");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::PreferFlowControl))
	{
		args.push_back(L"-Gfp");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::ResourcesMayAlias))
	{
		args.push_back(L"-res-may-alias");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::SkipOptimization))
	{
		args.push_back(L"-Od");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::SkipValidation))
	{
		args.push_back(L"-Vd");
	}

	if (Enum::EnumHasMask(InJob.Flags, EShaderCompileFlags::WarningsAsErrors))
	{
		args.push_back(L"-WX");
	}

	for (const auto& define : InJob.Defines)
	{
		args.push_back(L"-D");
		args.push_back(std::format(L"{}={}", ToWString(define.Name), ToWString(define.Definition)));
	}

	for (auto&& extra : InJob.AdditionalFlags)
	{
		args.push_back(std::move(extra));
	}

	std::vector<LPCWSTR> rawArgs;
	rawArgs.reserve(args.size());
	for (const auto& arg : args)
	{
		rawArgs.push_back(arg.c_str());
	}

	ComPtr<IDxcResult> results;
	compiler->Compile(&sourceBuffer, rawArgs.data(), static_cast<uint32_t>(rawArgs.size()), includeHandler.Get(), IID_PPV_ARGS(results.GetAddressOf()));

	ComPtr<IDxcBlobUtf8> errorBuffer;
	results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBuffer.GetAddressOf()), nullptr);

	if (errorBuffer && errorBuffer->GetStringLength() > 0)
	{
		errors.push_back(errorBuffer->GetStringPointer());
	}
	
	return errors;
}

std::vector<std::string> CompileShader(const ShaderCompilationJobDesc& InJob)
{
	return CompileShaderDXC(InJob);
}