#include "/Test/Public/ShaderTestFramework.hlsli"

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_UnitCubeAndZeroIndex_WHEN_Flattened_THEN_ReturnsZero(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 dim = uint3(1,1,1);
    const uint3 index = uint3(0,0,0);
    const uint flattened = STF::FlattenIndex(index, dim); 
    
    STF::AreEqual(0u, flattened);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_CubeSide10AndZeroIndex_WHEN_Flattened_THEN_ReturnsZero(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 dim = uint3(10,10,10);
    const uint3 index = uint3(0,0,0);
    const uint flattened = STF::FlattenIndex(index, dim); 
    
    STF::AreEqual(0u, flattened);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_CubeSide10AndIndicesDifferentByOneInX_WHEN_BothFlattened_THEN_DifferenceIsOne(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 dim = uint3(10,10,10);
    const uint3 index1 = uint3(0,0,0);
    const uint flattened1 = STF::FlattenIndex(index1, dim);

    const uint3 index2 = uint3(1,0,0);
    const uint flattened2 = STF::FlattenIndex(index2, dim);  
    
    STF::AreEqual(1u, flattened2 - flattened1);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_CubeSide10AndIndicesDifferentByOneInY_WHEN_BothFlattened_THEN_DifferenceIsTen(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 dim = uint3(10,10,10);
    const uint3 index1 = uint3(0,0,0);
    const uint flattened1 = STF::FlattenIndex(index1, dim);

    const uint3 index2 = uint3(0,1,0);
    const uint flattened2 = STF::FlattenIndex(index2, dim);  
    
    STF::AreEqual(10u, flattened2 - flattened1);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_CubeSide10AndIndicesDifferentByOneInZ_WHEN_BothFlattened_THEN_DifferenceIs100(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 dim = uint3(10,10,10);
    const uint3 index1 = uint3(0,0,0);
    const uint flattened1 = STF::FlattenIndex(index1, dim);

    const uint3 index2 = uint3(0,0,1);
    const uint flattened2 = STF::FlattenIndex(index2, dim);  
    
    STF::AreEqual(100u, flattened2 - flattened1);
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void GIVEN_CubeSide3_WHEN_IndicesIncrementedInXThenYThenZ_AND_WHEN_Flattened_THEN_DifferenceIs1(uint3 DispatchThreadId : SV_DispatchThreadID)
{
    const uint3 dim = uint3(3,3,3);
    uint3 prevIndex = uint3(0,0,0);
    
    
    for(uint z = 0; z < 3; ++z)
    {
        for (uint y = 0; y < 3; ++y)
        {
            const uint startX = y == 0 && z == 0 ? 1 : 0;
            for(uint x = startX; x < 3; ++x)
            {
                const uint3 nextIndex = uint3(x,y,z);
                const uint flattenPrev = STF::FlattenIndex(prevIndex, dim);
                const uint flattenNext = STF::FlattenIndex(nextIndex, dim);
                STF::AreEqual(1u, flattenNext - flattenPrev);
                prevIndex = nextIndex;
            }
        }
    }
}