#include "vertex_out/vertex_out_base.hlsli"
#include "feature/shadow_mapping.hlsli"

cbuffer ShadowConfig : register(b0)
{
    float4x4 g_shadow_view_proj_mat;
    float3 g_light_dir;
    float _shadow_padding;
};

Texture2D g_shadow_texture : register(t5);
SamplerComparisonState shadow_sampler : register(s1);

float4 main(VertexOut pixel_in) : SV_TARGET
{
    // shadow mapping
    float4 position_light = mul(pixel_in.position_w, g_shadow_view_proj_mat);
    float percent_lit = ComputeShadow(g_shadow_texture, shadow_sampler,
        position_light, pixel_in.normal_w, g_light_dir);
    //percent_lit = step(0.5f, percent_lit); // hard shadow

    return float4(0.015, 0.01, 0.01, 1.0 - percent_lit); // visualize shadow
}