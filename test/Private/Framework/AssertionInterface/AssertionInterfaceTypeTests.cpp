
#include "Framework/AssertionInterface.h"

#include "D3D12/CommandEngine.h"
#include "TestUtilities/EnumToTypeMap.h"
#include "TestUtilities/UniqueType.h"

#include "Utility/Type.h"

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

        EParamType ConstructorParamType = EParamType::Value;

        EReturnType CreateGPUResourcesReturnType = EReturnType::CorrectType;
        EParamType CreateGPUResourcesParamType = EParamType::NonConstRef;

        EReturnType BindShaderDataReturnType = EReturnType::CorrectType;
        EParamType BindShaderDataContextParamType = EParamType::NonConstRef;
        EParamType BindShaderDataResourcesParamType = EParamType::ConstRef;

        EReturnType QueueReadbacksReturnType = EReturnType::CorrectType;
        EParamType QueueReadbacksParamType = EParamType::ConstRef;
    };

    template<TypeSpecifiersType TypeSpecifiers = TypeSpecifiersType{}>
    struct TestInterface
    {
        using TestRunResultsType = std::conditional_t<TypeSpecifiers.ValidTestRunResultsType, ValidTestRunResultsType, UniqueType<Empty>>;
        using GPUResourcesType = UniqueType<Empty>;
        using CreationParams = UniqueType<Empty>;

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

        using ConstructorParamType = ParamTypeMapping<CreationParams, Empty>::template FindTypeOr<TypeSpecifiers.ConstructorParamType, UniqueType<Empty>>;

        using CreateGPUResourcesParamType = ParamTypeMapping<ScopedCommandContext, UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesParamType, UniqueType<Empty>>;
        using CreateGPUResourcesReturnType = ReturnTypeMapping<ExpectedError<GPUResourcesType>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.CreateGPUResourcesReturnType, UniqueType<Empty>>;
        using BindShaderDataContextParamType = ParamTypeMapping<ScopedCommandShader, UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.BindShaderDataContextParamType, UniqueType<Empty>>;
        using BindShaderDataResourceParamType = ParamTypeMapping<GPUResourcesType, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.BindShaderDataResourcesParamType, UniqueType<Empty>>;
        using BindShaderDataReturnType = ReturnTypeMapping<ExpectedError<void>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.BindShaderDataReturnType, UniqueType<Empty>>;

        using QueueReadbacksParamType = ParamTypeMapping<GPUResourcesType, const UniqueType<Empty>&>::template FindTypeOr<TypeSpecifiers.QueueReadbacksParamType, UniqueType<Empty>>;
        using QueueReadbacksReturnType = ReturnTypeMapping<ExpectedError<GPUReadbackResourcesType>, UniqueType<Empty>>::template FindTypeOr<TypeSpecifiers.QueueReadbacksReturnType, UniqueType<Empty>>;

        TestInterface(ConstructorParamType)
        {
        }

        CreateGPUResourcesReturnType CreateGPUResources(CreateGPUResourcesParamType)
        {
            return CreateGPUResourcesReturnType{};
        }
        
        BindShaderDataReturnType BindShaderData(BindShaderDataContextParamType, BindShaderDataResourceParamType)
        {
            return BindShaderDataReturnType{};
        }

        QueueReadbacksReturnType QueueReadbacks(QueueReadbacksParamType)
        {
            return QueueReadbacksReturnType{};
        }
    };

    static_assert( CAssertionInterfaceType< TestInterface <> >, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ValidTestRunResultsType = false } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .ConstructorParamType = EParamType::NonConstRef } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesReturnType = EReturnType::WrongType } >>, "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::ConstRef } >>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::Value } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::RValueRef } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = EParamType::WrongType } >>, "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataReturnType = EReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataContextParamType = EParamType::ConstRef } >> , "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataContextParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataContextParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataContextParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::NonConstRef } >> , "Expected this type to be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .BindShaderDataResourcesParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");

    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksReturnType = EReturnType::WrongType } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksParamType = EParamType::NonConstRef } >> , "Expected this type to be valid for the concept");
    static_assert( CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksParamType = EParamType::Value } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksParamType = EParamType::RValueRef } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType < TestInterface < TypeSpecifiersType{ .QueueReadbacksParamType = EParamType::WrongType } >> , "Expected this type to not be valid for the concept");
}