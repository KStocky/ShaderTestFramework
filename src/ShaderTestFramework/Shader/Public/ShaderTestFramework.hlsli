#pragma once

#define SHADER_TEST_RS \
"RootFlags(" \
    "DENY_VERTEX_SHADER_ROOT_ACCESS |" \
    "DENY_HULL_SHADER_ROOT_ACCESS |" \
    "DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
    "DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
    "DENY_PIXEL_SHADER_ROOT_ACCESS |" \
    "DENY_AMPLIFICATION_SHADER_ROOT_ACCESS |" \
    "DENY_MESH_SHADER_ROOT_ACCESS |" \
    "CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED" \
")," \
"RootConstants(" \
    "num32BitConstants=2," \
    "b0" \
")"

namespace ShaderTestPrivate
{
    const uint MaxNumAsserts;
    const uint AssertBufferIndex;
    static const uint AssertFailureNumBytes = 16;
    
    RWByteAddressBuffer GetAssertBuffer()
    {
        return ResourceDescriptorHeap[AssertBufferIndex];
    }
    
    void Success()
    {
        uint successIndex;
        GetAssertBuffer().InterlockedAdd(0, 1, successIndex);
    }
    
    void AddError(const uint InCode, const uint InType, const uint InActual, const uint InExpected)
    {
        uint assertIndex;
        GetAssertBuffer().InterlockedAdd(4, 1, assertIndex);
        
        if (assertIndex < MaxNumAsserts)
        {
            const uint byteOffset = 8 + assertIndex * AssertFailureNumBytes;
            GetAssertBuffer().Store4(byteOffset, uint4(InCode, InType, InActual, InExpected));
        }
    }
}