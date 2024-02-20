#include "D3D12/Shader/ShaderCompilerTestsCommon.h"
#include <Utility/EnumReflection.h>

#include <format>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

using namespace ShaderCompilerTestsCommon;

SCENARIO("HLSLTests")
{
    auto [name, successCondition, flags] =
        GENERATE
        (
            table<std::string, bool(*)(const EHLSLVersion), std::vector<std::wstring>>
            (
                { 
                    std::tuple
                    {
                        "FunctionStyleMacro",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "VariadicMacro",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "GlobalString",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "LocalStringAssignedToAUintArray",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "LocalString",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                     std::tuple
                    {
                        "OperatorBoolWithAnEmptyStruct",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "OperatorIntWithAnEmptyStruct",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "OperatorBool",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "OperatorInt",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "CounterMacro",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "LineMacro",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "StaticStructMemberInTemplatedStruct",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "StaticVariableInTemplatedFunction",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "StaticConstStructMemberInTemplatedStruct",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "StaticStructMemberInNonTemplatedStruct",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "MacroGeneratedStructWithStaticDataMember",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "ForwardDeclareFunction",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "ForwardDeclareFunctionInOtherFunction",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "ForwardDeclareFunctionInOtherFunctionAndDefineIt",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "DefineStructMemberFunctionLater",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "DefineStructMemberFunctionInFunction",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "CleanupAttribute",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "SizeofOperator",
                        [](const EHLSLVersion) { return true; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "HasMemberCheck",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "ArrayTemplateSpecialization",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "NestedStructTemplates",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "TernaryInTypeTrait",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "DeferSFINAE",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "GloballyCoherentInStruct",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "GloballyCoherentTypeModifier",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "CallOperator",
                        [](const EHLSLVersion InVer) { return InVer == EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "Alignof",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "DeducingPackingOrder",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "UserDefinedVector",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "SizeofEnumType",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "IsEnum",
                        [](const EHLSLVersion) { return false; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "ExpressionSFINAE",
                        [](const EHLSLVersion InVer) { return InVer >= EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "StaticAssert",
                        [](const EHLSLVersion InVer) { return InVer >= EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "PrinterWithOptimizations",
                        [](const EHLSLVersion InVer) { return InVer >= EHLSLVersion::v2021; },
                        std::vector<std::wstring>{}
                    },
                    std::tuple
                    {
                        "PrinterWithoutOptimizations",
                        [](const EHLSLVersion InVer) { return InVer >= EHLSLVersion::v2021; },
                        std::vector<std::wstring>{L"-Od", L"-D", L"OPTIMIZATIONS_ENABLED=0"}
                    }
                }
            )
        );

    const auto hlslVersion = GENERATE(
        EHLSLVersion::v2016,
        EHLSLVersion::v2017,
        EHLSLVersion::v2018,
        EHLSLVersion::v2021
    );

    auto compiler = CreateCompiler();

    GIVEN(name)
    {
        WHEN("Compiled with HLSL version " << Enum::UnscopedName(hlslVersion))
        {
            const auto job = CreateCompilationJob(EShaderType::Compute, D3D_SHADER_MODEL_6_7, hlslVersion, std::move(flags), std::format("/Tests/HLSLTests/{}.hlsl", name));
            const auto errors = compiler.CompileShader(job);
            if (successCondition(hlslVersion))
            {
                THEN("Compilation Succeeds")
                {
                    const auto error = errors.has_value() ? "" : errors.error();
                    CAPTURE(error);
                    REQUIRE(errors.has_value());
                }
            }
            else
            {
                THEN("Compilation Fails")
                {
                    REQUIRE_FALSE(errors.has_value());
                }
            }
        }
    }
}