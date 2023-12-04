Texture2D<float4> g_texture : register(t0);

SamplerState g_sampler : register(s0);
FeedbackTexture2D<SAMPLER_FEEDBACK_MIP_REGION_USED> g_feedback : register(u3);

float4 Main() : SV_TARGET
{
    //float2 uv = in.uv;
    g_feedback.WriteSamplerFeedback(g_texture, g_sampler, float2(0.0, 0.5));

    return g_texture.Sample(g_sampler, float2(0.5,0.5));
}