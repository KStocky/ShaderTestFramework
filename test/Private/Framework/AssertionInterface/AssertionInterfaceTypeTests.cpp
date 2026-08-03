
#include "Framework/AssertionInterface.h"

#include "D3D12/CommandEngine.h"
#include "TestUtilities/EnumToTypeMap.h"
#include "TestUtilities/UniqueType.h"

#include "Utility/Type.h"

#include <string>
#include <vector>

namespace stf::assert::AssertionInterfaceTypeTests
{
    struct Empty {};

    struct ValidTestRunResultsType
    {
        bool Succeeded() const { return true; }
        friend bool operator==(const ValidTestRunResultsType&, const ValidTestRunResultsType&) { return false; };
        friend bool operator!=(const ValidTestRunResultsType&, const ValidTestRunResultsType&) { return true; };

        friend std::ostream& operator<<(std::ostream& InOut, const ValidTestRunResultsType&)
        {
            return InOut;
        }
    };

    enum class EParamType : u8
    {
        NonConstRef,
        ConstRef,
        RValueRef,
        Value,
        WrongType
    };

    enum class EReturnType : u8
    {
        CorrectType,
        WrongType
    };

    struct TypeSpecifiersType
    {
        bool ValidTestRunResultsType = true;
        bool ValidAssertionLibraryVirtualPath = true;

        EReturnType GetAdditionalCompilerArgsReturnType = EReturnType::CorrectType;

        EReturnType CreateGPUResourcesReturnType = EReturnType::CorrectType;
        EParamType CreateGPUResourcesContextParamType = EParamType::NonConstRef;
        EParamType CreateGPUResourcesPerTestDataParamType = EParamType::ConstRef;

        EReturnType BindShaderDataReturnType = EReturnType::CorrectType;
        EParamType BindShaderDataShaderParamType = EParamType::NonConstRef;
        EParamType BindShaderDataResourcesParamType = EParamType::ConstRef;
        EParamType BindShaderDataPerTestDataParamType = EParamType::ConstRef;

        EReturnType QueueReadbacksReturnType = EReturnType::CorrectType;
        EParamType QueueReadbacksResourcesParamType = EParamType::ConstRef;
        EParamType QueueReadbacksContextParamType = EParamType::NonConstRef;

        EReturnType ProcessReadbacksReturnType = EReturnType::CorrectType;
        EParamType ProcessReadbacksReadbackParamType = EParamType::ConstRef;
        EParamType ProcessReadbacksEngineParamType = EParamType::NonConstRef;
        EParamType ProcessReadbacksPerTestDataParamType = EParamType::ConstRef;
    };

    template<TypeSpecifiersType TypeSpecifiers = TypeSpecifiersType{}>
    struct TestInterface
    {
    public:

        using AssertionLibraryVirtualPathType = std::conditional_t<TypeSpecifiers.ValidAssertionLibraryVirtualPath, StringLiteral, UniqueType<StringLiteral>>;

        static constexpr AssertionLibraryVirtualPathType AssertionLibraryVirtualPath{ std::string_view{} };

        using TestRunResultsType = std::conditional_t<TypeSpecifiers.ValidTestRunResultsType, ValidTestRunResultsType, UniqueType<Empty>>;
        using GPUResourcesType = UniqueType<Empty>;
        using PerTestData = UniqueType<Empty>;

        using GPUReadbackResourcesType = UniqueType<Empty>;

        template<typename BaseType, typename WrongType>
        using ParamTypeMapping =
            EnumValsToTypes<
                EnumToType<EParamType::NonConstRef, BaseType&>,
                EnumToType<EParamType::ConstRef, const BaseType&>,
                EnumToType<EParamType::RValueRef, BaseType&&>,
                EnumToType<EParamType::Value, BaseType>,
                EnumToType<EParamType::WrongType, WrongType>
            >;

        template<typename CorrectType, typename WrongType>
        using ReturnTypeMapping =
            EnumValsToTypes<
                EnumToType<EReturnType::CorrectType, CorrectType>,
                EnumToType<EReturnType::WrongType, WrongType>
            >;

        using GetAdditionalCompilerArgsReturnType = ReturnTypeMapping<std::vector<std::wstring>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.GetAdditionalCompilerArgsReturnType, UniqueType<Empty>>;

        using CreateGPUResourcesContextParamType = ParamTypeMapping<ScopedCommandContext, UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesContextParamType, UniqueType<Empty>>;
        using CreateGPUResourcesPerTestDataParamType = ParamTypeMapping<PerTestData, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesPerTestDataParamType, UniqueType<Empty>>;
        using CreateGPUResourcesReturnType = ReturnTypeMapping<ExpectedError<GPUResourcesType>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesReturnType, UniqueType<Empty>>;
        using BindShaderDataShaderParamType = ParamTypeMapping<ScopedCommandShader, UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.BindShaderDataShaderParamType, UniqueType<Empty>>;
        using BindShaderDataResourceParamType = ParamTypeMapping<GPUResourcesType, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.BindShaderDataResourcesParamType, UniqueType<Empty>>;
        using BindShaderDataPerTestDataParamType = ParamTypeMapping<PerTestData, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.BindShaderDataPerTestDataParamType, UniqueType<Empty>>;
        using BindShaderDataReturnType = ReturnTypeMapping<ExpectedError<void>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.BindShaderDataReturnType, UniqueType<Empty>>;

        using QueueReadbacksResourcesParamType = ParamTypeMapping<GPUResourcesType, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.QueueReadbacksResourcesParamType, UniqueType<Empty>>;
        using QueueReadbacksContextParamType = ParamTypeMapping<ScopedCommandContext, UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.QueueReadbacksContextParamType, UniqueType<Empty>>;
        using QueueReadbacksReturnType = ReturnTypeMapping<ExpectedError<GPUReadbackResourcesType>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.QueueReadbacksReturnType, UniqueType<Empty>>;

        using ProcessReadbacksReadbackParamType = ParamTypeMapping<GPUReadbackResourcesType, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.ProcessReadbacksReadbackParamType, UniqueType<Empty>>;
        using ProcessReadbacksEngineParamType = ParamTypeMapping<CommandEngine, UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.ProcessReadbacksEngineParamType, UniqueType<Empty>>;
        using ProcessReadbacksPerTestDataParamType = ParamTypeMapping<PerTestData, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.ProcessReadbacksPerTestDataParamType, UniqueType<Empty>>;
        using ProcessReadbacksReturnType = ReturnTypeMapping<ExpectedError<TestRunResultsType>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.ProcessReadbacksReturnType, UniqueType<Empty>>;

        GetAdditionalCompilerArgsReturnType GetAdditionalCompilerArgs() const
        {
            return GetAdditionalCompilerArgsReturnType{};
        }

        CreateGPUResourcesReturnType CreateGPUResources(CreateGPUResourcesContextParamType, CreateGPUResourcesPerTestDataParamType)
        {
            return CreateGPUResourcesReturnType{};
        }
        
        BindShaderDataReturnType BindShaderData(BindShaderDataShaderParamType, BindShaderDataResourceParamType, BindShaderDataPerTestDataParamType)
        {
            return BindShaderDataReturnType{};
        }

        QueueReadbacksReturnType QueueReadbacks(QueueReadbacksContextParamType, QueueReadbacksResourcesParamType)
        {
            return QueueReadbacksReturnType{};
        }

        ProcessReadbacksReturnType ProcessReadbacks(ProcessReadbacksEngineParamType, ProcessReadbacksReadbackParamType, ProcessReadbacksPerTestDataParamType)
        {
            return ProcessReadbacksReturnType{};
        }
    };

    static_assert( CAssertionInterfaceType< TestInterface <> >, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ValidTestRunResultsType = false } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ValidAssertionLibraryVirtualPath = false } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .GetAdditionalCompilerArgsReturnType = EReturnType::WrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesReturnType = EReturnType::WrongType } >>, "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesContextParamType = EParamType::ConstRef } >>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesContextParamType = EParamType::Value } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesContextParamType = EParamType::RValueRef } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesContextParamType = EParamType::WrongType } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesPerTestDataParamType = EParamType::NonConstRef } >>, "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesPerTestDataParamType = EParamType::Value } >>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesPerTestDataParamType = EParamType::RValueRef } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesPerTestDataParamType = EParamType::WrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataReturnType = EReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataShaderParamType = EParamType::ConstRef } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataShaderParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataShaderParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataShaderParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::NonConstRef } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::Value } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataPerTestDataParamType = EParamType::NonConstRef } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataPerTestDataParamType = EParamType::Value } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataPerTestDataParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataPerTestDataParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksReturnType = EReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksResourcesParamType = EParamType::NonConstRef } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksResourcesParamType = EParamType::Value } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksResourcesParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksResourcesParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");

    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksContextParamType = EParamType::ConstRef } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksContextParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksContextParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksContextParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksReturnType = EReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksReadbackParamType = EParamType::NonConstRef } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksReadbackParamType = EParamType::Value } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksReadbackParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksReadbackParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");

    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksEngineParamType = EParamType::ConstRef } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksEngineParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksEngineParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksEngineParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksPerTestDataParamType = EParamType::NonConstRef } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksPerTestDataParamType = EParamType::Value } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksPerTestDataParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ProcessReadbacksPerTestDataParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");
}