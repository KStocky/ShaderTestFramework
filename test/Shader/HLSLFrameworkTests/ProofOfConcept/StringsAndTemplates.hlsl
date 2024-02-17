#include "/Test/STF/ShaderTestFramework.hlsli"

template<typename T, uint N>
uint ArrayLen(T In[N])
{
    return N;
}

[RootSignature(SHADER_TEST_RS)]
[numthreads(1,1,1)]
void StringsAndTemplates()
{
    ASSERT(AreEqual, ArrayLen("Hello"), 6u);
}