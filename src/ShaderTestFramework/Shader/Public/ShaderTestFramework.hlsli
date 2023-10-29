
namespace ShaderTestPrivate
{
    RWByteAddressBuffer AssertBuffer;
    const uint MaxNumAsserts;
    static const uint AssertFailureNumBytes = 16;
    
    void Success()
    {
        uint successIndex;
        AssertBuffer.InterlockedAdd(0, 1, successIndex);
    }
    
    void AddError(const uint InCode, const uint InType, const uint InActual, const uint InExpected)
    {
        uint assertIndex;
        AssertBuffer.InterlockedAdd(4, 1, assertIndex);
        
        if (assertIndex < MaxNumAsserts)
        {
            const uint byteOffset = 8 + assertIndex * AssertFailureNumBytes;
            AssertBuffer.Store4(byteOffset, uint4(InCode, InType, InActual, InExpected));
        }

    }
}