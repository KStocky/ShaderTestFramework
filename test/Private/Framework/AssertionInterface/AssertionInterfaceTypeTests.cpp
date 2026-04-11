
#include "Framework/AssertionInterface.h"

#include "D3D12/CommandEngine.h"
#include "TestUtilities/EnumToTypeMap.h"
#include "TestUtilities/UniqueType.h"

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
        NonConstRefWrongType,
        ConstRef,
        RValueRef,
        Value
    };

    enum class EReturnType : u8
    {
        CorrectType,
        WrongType
    };

    enum class EConstructorParamType : u8
    {
        CorrectType,
        WrongType
    };

    struct TypeSpecifiersType
    {
        bool ValidTestRunResultsType = true;
        EReturnType CreateGPUResourcesReturnType = EReturnType::CorrectType;
        EParamType CreateGPUResourcesParamType = EParamType::NonConstRef;
        EConstructorParamType ConstructorParamType = EConstructorParamType::CorrectType;
        EReturnType BindShaderDataReturnType = EReturnType::CorrectType;
        EParamType BindShaderDataParamType = EParamType::NonConstRef;

    };

    template<TypeSpecifiersType TypeSpecifiers = TypeSpecifiersType{}>
    struct TestInterface
    {
        using TestRunResultsType = std::conditional_t<TypeSpecifiers.ValidTestRunResultsType, ValidTestRunResultsType, UniqueType<Empty>>;
        using GPUResourcesType = UniqueType<Empty>;
        using CreationParams = UniqueType<Empty>;

        using CreateGPUResourcesParamTypeMapping =
            EnumValsToTypes<
                EnumToType<EParamType::NonConstRef, ScopedCommandContext&>,
                EnumToType<EParamType::ConstRef, const ScopedCommandContext&>,
                EnumToType<EParamType::RValueRef, ScopedCommandContext&&>,
                EnumToType<EParamType::Value, ScopedCommandContext>,
                EnumToType<EParamType::NonConstRefWrongType, Empty&>
            >;

        using CreateGPUResourcesParamType = CreateGPUResourcesParamTypeMapping::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesParamType, UniqueType<Empty>>;

        using CreateGPUResourcesReturnTypeMapping =
            EnumValsToTypes<
                EnumToType<EReturnType::CorrectType, ExpectedError<GPUResourcesType>>,
                EnumToType<EReturnType::WrongType, UniqueType<Empty>>
            >;
        using CreateGPUResourcesReturnType = CreateGPUResourcesReturnTypeMapping::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesReturnType, UniqueType<Empty>>;

        using ConstructorParamTypeMapping =
            EnumValsToTypes<
                EnumToType<EConstructorParamType::CorrectType, CreationParams>,
                EnumToType<EConstructorParamType::WrongType, UniqueType<Empty>>
            >;

        using ConstructorParamType = ConstructorParamTypeMapping::template FindTypeOr<TypeSpecifiers.ConstructorParamType, UniqueType<Empty>>;

        using BindShaderDataParamTypeMapping =
            EnumValsToTypes<
                EnumToType<EParamType::NonConstRef, ScopedCommandShader&>,
                EnumToType<EParamType::ConstRef, const ScopedCommandShader&>,
                EnumToType<EParamType::RValueRef, ScopedCommandShader&&>,
                EnumToType<EParamType::Value, ScopedCommandShader>,
                EnumToType<EParamType::NonConstRefWrongType, Empty&>
            >;

        using BindShaderDataParamType = BindShaderDataParamTypeMapping::template FindTypeOr<TypeSpecifiers.BindShaderDataParamType, UniqueType<Empty>>;

        using BindShaderDataReturnTypeMapping =
            EnumValsToTypes<
                EnumToType<EReturnType::CorrectType, ExpectedError<void>>,
                EnumToType<EReturnType::WrongType, UniqueType<Empty>>
            >;
        using BindShaderDataReturnType = BindShaderDataReturnTypeMapping::template FindTypeOr<TypeSpecifiers.BindShaderDataReturnType, UniqueType<Empty>>;

        TestInterface(ConstructorParamType)
        {
        }

        CreateGPUResourcesReturnType CreateGPUResources(CreateGPUResourcesParamType)
        {
            return CreateGPUResourcesReturnType{};
        }

        BindShaderDataReturnType BindShaderData(BindShaderDataParamType)
        {
            return BindShaderDataReturnType{};
        }
    };

    static_assert(CAssertionInterfaceType<TestInterface<>>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ValidTestRunResultsType = false } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesReturnType = EReturnType::WrongType } >>, "Expected this type to not be valid for the concept");
    static_assert(CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::ConstRef } >>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::Value } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::RValueRef } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::NonConstRefWrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ConstructorParamType = EConstructorParamType::WrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataReturnType = EReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EParamType::ConstRef } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EParamType::NonConstRefWrongType } >> , "Expected this type to not be valid for the concept");
}