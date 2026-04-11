
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

    enum class ECreateGPUResourcesParamType : u8
    {
        NonConstRef,
        NonConstRefWrongType,
        ConstRef,
        RValueRef,
        Value
    };

    enum class ECreateGPUResourcesReturnType : u8
    {
        CorrectType,
        WrongType
    };

    enum class EConstructorParamType : u8
    {
        CorrectType,
        WrongType
    };

    enum class EBindShaderDataParamType : u8
    {
        NonConstRef,
        NonConstRefWrongType,
        ConstRef,
        RValueRef,
        Value
    };

    enum class EBindShaderDataReturnType : u8
    {
        CorrectType,
        WrongType
    };

    struct TypeSpecifiersType
    {
        bool ValidTestRunResultsType = true;
        ECreateGPUResourcesReturnType CreateGPUResourcesReturnType = ECreateGPUResourcesReturnType::CorrectType;
        ECreateGPUResourcesParamType CreateGPUResourcesParamType = ECreateGPUResourcesParamType::NonConstRef;
        EConstructorParamType ConstructorParamType = EConstructorParamType::CorrectType;
        EBindShaderDataReturnType BindShaderDataReturnType = EBindShaderDataReturnType::CorrectType;
        EBindShaderDataParamType BindShaderDataParamType = EBindShaderDataParamType::NonConstRef;

    };

    template<TypeSpecifiersType TypeSpecifiers = TypeSpecifiersType{}>
    struct TestInterface
    {
        using TestRunResultsType = std::conditional_t<TypeSpecifiers.ValidTestRunResultsType, ValidTestRunResultsType, UniqueType<Empty>>;
        using GPUResourcesType = UniqueType<Empty>;
        using CreationParams = UniqueType<Empty>;

        using CreateGPUResourcesParamTypeMapping =
            EnumValsToTypes<
                EnumToType<ECreateGPUResourcesParamType::NonConstRef, ScopedCommandContext&>,
                EnumToType<ECreateGPUResourcesParamType::ConstRef, const ScopedCommandContext&>,
                EnumToType<ECreateGPUResourcesParamType::RValueRef, ScopedCommandContext&&>,
                EnumToType<ECreateGPUResourcesParamType::Value, ScopedCommandContext>,
                EnumToType<ECreateGPUResourcesParamType::NonConstRefWrongType, Empty&>
            >;

        using CreateGPUResourcesParamType = CreateGPUResourcesParamTypeMapping::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesParamType, UniqueType<Empty>>;

        using CreateGPUResourcesReturnTypeMapping =
            EnumValsToTypes<
                EnumToType<ECreateGPUResourcesReturnType::CorrectType, ExpectedError<GPUResourcesType>>,
                EnumToType<ECreateGPUResourcesReturnType::WrongType, UniqueType<Empty>>
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
                EnumToType<EBindShaderDataParamType::NonConstRef, ScopedCommandShader&>,
                EnumToType<EBindShaderDataParamType::ConstRef, const ScopedCommandShader&>,
                EnumToType<EBindShaderDataParamType::RValueRef, ScopedCommandShader&&>,
                EnumToType<EBindShaderDataParamType::Value, ScopedCommandShader>,
                EnumToType<EBindShaderDataParamType::NonConstRefWrongType, Empty&>
            >;

        using BindShaderDataParamType = BindShaderDataParamTypeMapping::template FindTypeOr<TypeSpecifiers.BindShaderDataParamType, UniqueType<Empty>>;

        using BindShaderDataReturnTypeMapping =
            EnumValsToTypes<
                EnumToType<EBindShaderDataReturnType::CorrectType, ExpectedError<void>>,
                EnumToType<EBindShaderDataReturnType::WrongType, UniqueType<Empty>>
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

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesReturnType = ECreateGPUResourcesReturnType::WrongType } >>, "Expected this type to not be valid for the concept");
    static_assert(CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::ConstRef } >>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::Value } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::RValueRef } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::NonConstRefWrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ConstructorParamType = EConstructorParamType::WrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataReturnType = EBindShaderDataReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EBindShaderDataParamType::ConstRef } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EBindShaderDataParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EBindShaderDataParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataParamType = EBindShaderDataParamType::NonConstRefWrongType } >> , "Expected this type to not be valid for the concept");
}