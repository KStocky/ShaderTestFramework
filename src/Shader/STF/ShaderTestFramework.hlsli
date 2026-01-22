#ifndef STF_SHADER_TEST_FRAMEWORK_HEADER
#define STF_SHADER_TEST_FRAMEWORK_HEADER

#include "/Test/STF/AssertionsV1/Assertions.hlsli"
#include "/Test/STF/AssertionsV1/Sections.hlsli"
#include "/Test/STF/AssertionsV1/Strings.hlsli"
#include "/Test/STF/SectionManagement.hlsli"

#include "/Test/TTL/macro.hlsli"
#include "/Test/TTL/type_traits.hlsli"

namespace stf
{
    void RegisterThreadID(uint InID)
    {
        detail::Scratch.ThreadID.Type = detail::EThreadIDType::Int;
        detail::Scratch.ThreadID.Data = InID;
    }

    void RegisterThreadID(uint3 InID)
    {
        detail::Scratch.ThreadID.Type = detail::EThreadIDType::Int3;
        detail::Scratch.ThreadID.Data = detail::FlattenIndex(InID, AssertionsV1::detail::DispatchDimensions);
    }
}

#endif
