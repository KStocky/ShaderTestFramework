#include "D3D12/Shader/ShaderCompiler.h"

#include "D3D12/Shader/IncludeHandler.h"

#include "Utility/EnumReflection.h"
#include "Utility/Exception.h"
#include "Utility/OverloadSet.h"
#include "Utility/Pointer.h"

#include <fstream>
#include <ranges>
#include <variant>

#include <dxcapi.h>
#include <d3d12shader.h>

namespace
{
	std::wstring ToWString(const std::string& InString)
	{
		return std::wstring(InString.cbegin(), InString.cend());
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

}

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

CompilationResult ShaderCompiler::CompileShader(const ShaderCompilationJobDesc& InJob)
{
	const auto source = InJob.Source.ToString();

	ComPtr<IDxcUtils> utils;
	ComPtr<IDxcCompiler3> compiler;
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));

	const auto shaderLibraryPath = fs::current_path() / fs::path(SHADER_SRC);

	std::vector mappings =
	{
		IncludeHandler::Mapping
		{
			"/Test",
			shaderLibraryPath.generic_string()
		}
	};

	ComPtr<IDxcIncludeHandler> includeHandler = new IncludeHandler(std::move(mappings), utils);

	DxcBuffer sourceBuffer;
	sourceBuffer.Encoding = DXC_CP_ACP;
	sourceBuffer.Ptr = source.c_str();
	sourceBuffer.Size = source.size();

	std::vector<std::wstring> args;

	args.push_back(L"-E");
	args.push_back(ToWString(InJob.EntryPoint));
	args.push_back(L"-T");
	args.push_back(ToWString(MakeShaderTarget(InJob.ShaderModel, InJob.ShaderType)));

	args.push_back(L"-Zs");
	args.push_back(L"-Zss");

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

	for (const auto& extra : InJob.AdditionalFlags)
	{
		args.push_back(extra);
	}

	std::vector<LPCWSTR> rawArgs;
	rawArgs.reserve(args.size());
	for (const auto& arg : args)
	{
		rawArgs.push_back(arg.c_str());
	}

	ComPtr<IDxcResult> results;
	ThrowIfFailed(compiler->Compile(&sourceBuffer, rawArgs.data(), static_cast<u32>(rawArgs.size()), includeHandler.Get(), IID_PPV_ARGS(results.GetAddressOf())));

	// GetOutputByIndex can not be trusted to return DXC_OUT_ERRORS in all cases
	// So we have to handle errors separately
	// made a bug here https://github.com/microsoft/DirectXShaderCompiler/issues/5923
	if (results->HasOutput(DXC_OUT_ERRORS))
	{
		ComPtr<IDxcBlobUtf8> errorBuffer;
		ThrowIfFailed(results->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBuffer.GetAddressOf()), nullptr));
	
		if (errorBuffer && errorBuffer->GetStringLength() > 0)
		{
			return Unexpected{ std::string{errorBuffer->GetStringPointer()} };
		}
	}

	CompiledShaderData::CreationParams params;

	if (results->HasOutput(DXC_OUT_OBJECT))
	{
		ComPtr<IDxcBlob> objBlob;
		ThrowIfFailed(results->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(objBlob.GetAddressOf()), nullptr));
		params.CompiledShader = std::move(objBlob);
	}

	if (results->HasOutput(DXC_OUT_SHADER_HASH))
	{
		ComPtr<IDxcBlob> hashBlob;
		ThrowIfFailed(results->GetOutput(DXC_OUT_SHADER_HASH, IID_PPV_ARGS(hashBlob.GetAddressOf()), nullptr));
		DxcShaderHash hash;
		std::memcpy(&hash, hashBlob->GetBufferPointer(), hashBlob->GetBufferSize());
		params.Hash = hash;
	}

	if (InJob.ShaderType != EShaderType::Lib && results->HasOutput(DXC_OUT_REFLECTION))
	{
		ComPtr<IDxcBlob> blob;
		ThrowIfFailed(results->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(blob.GetAddressOf()), nullptr));
		const DxcBuffer reflectionBuffer
		{
			.Ptr = blob->GetBufferPointer(),
			.Size = blob->GetBufferSize(),
			.Encoding = 0
		};
		ComPtr<ID3D12ShaderReflection> shaderReflection;
		ThrowIfFailed(utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(shaderReflection.GetAddressOf())));

		params.Reflection = std::move(shaderReflection);
	}
	
	return CompiledShaderData{ ShaderCompilerToken{}, std::move(params)};
}
