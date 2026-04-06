
#include "Framework/AssertionInterface.h"

#include "D3D12/CommandEngine.h"
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

    enum class EConstructorParamType : u8
    {
        CorrectType,
        WrongType
    };

    enum class ECreateGPUResourcesReturnType : u8
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
    };

    template<TypeSpecifiersType TypeSpecifiers = TypeSpecifiersType{}>
    struct TestInterface
    {
        using TestRunResultsType = std::conditional_t<TypeSpecifiers.ValidTestRunResultsType, ValidTestRunResultsType, UniqueType<Empty>>;
        using GPUResourcesType = UniqueType<Empty>;
        using CreationParams = UniqueType<Empty>;

        using CreateGPUResourcesReturnType = std::conditional_t<
            TypeSpecifiers.CreateGPUResourcesReturnType == ECreateGPUResourcesReturnType::CorrectType,
            GPUResourcesType,
            UniqueType<Empty>
        >;

        using CreateGPUResourcesParamType = decltype(
            []()
            {
                if constexpr (TypeSpecifiers.CreateGPUResourcesParamType == ECreateGPUResourcesParamType::NonConstRef)
                {
                    return std::type_identity<ScopedCommandContext&>{};
                }
                else if constexpr (TypeSpecifiers.CreateGPUResourcesParamType == ECreateGPUResourcesParamType::ConstRef)
                {
                    return std::type_identity<const ScopedCommandContext&>{};
                }
                else if constexpr (TypeSpecifiers.CreateGPUResourcesParamType == ECreateGPUResourcesParamType::RValueRef)
                {
                    return std::type_identity<ScopedCommandContext&&>{};
                }
                else if constexpr (TypeSpecifiers.CreateGPUResourcesParamType == ECreateGPUResourcesParamType::Value)
                {
                    return std::type_identity<ScopedCommandContext>{};
                }
                else
                {
                    return std::type_identity<UniqueType<Empty&>>{};
                }
            }()
            )::type;

        using ConstructorParamType = decltype(
            []()
            {
                if constexpr (TypeSpecifiers.ConstructorParamType == EConstructorParamType::CorrectType)
                {
                    return CreationParams{};
                }
                else
                {
                    return UniqueType<Empty>{};
                }
            }()
            );

        TestInterface(ConstructorParamType)
        {
        }

        CreateGPUResourcesReturnType CreateGPUResources(CreateGPUResourcesParamType)
        {
            return CreateGPUResourcesReturnType{};
        }
    };

    static_assert(CAssertionInterfaceType<TestInterface<>>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ValidTestRunResultsType = false } >> , "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesReturnType = ECreateGPUResourcesReturnType::WrongType } >>, "Expected this type to not be valid for the concept");
    static_assert(CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::ConstRef } >>, "Expected this type to be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::Value } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .CreateGPUResourcesParamType = ECreateGPUResourcesParamType::RValueRef } >>, "Expected this type to not be valid for the concept");
    static_assert(!CAssertionInterfaceType< TestInterface < TypeSpecifiersType{ .ConstructorParamType = EConstructorParamType::WrongType } >>, "Expected this type to not be valid for the concept");
}