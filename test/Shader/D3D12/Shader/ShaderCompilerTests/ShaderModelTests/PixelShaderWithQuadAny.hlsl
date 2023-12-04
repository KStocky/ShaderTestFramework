SamplerState s0 : register(s0);
Texture2D t0 : register(t0);

float4 Main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
    float4 ret = 0;
    bool cond = pos.x > 500;

    if (QuadAny(cond))
    {
        float4 sampled_result = float4(1.0, 2.0, 3.0, 4.0);
        if (cond)
        {
            ret = sampled_result;
        }
    }
    return ret;
}