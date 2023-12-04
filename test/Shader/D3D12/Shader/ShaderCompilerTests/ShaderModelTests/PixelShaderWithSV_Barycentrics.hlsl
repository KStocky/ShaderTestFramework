float4 Main(float3 baryWeights : SV_Barycentrics) : SV_Target 
{
    return float4(baryWeights, 0.0);
}