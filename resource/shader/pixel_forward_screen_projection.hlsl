#include "vertex_out/vertex_out_fullscreen.hlsli"

cbuffer MaterialForwardProjector : register(b0)
{
    // projector space
    float4x4 g_projector_matrix_view_proj;
    float g_shape_height;
    float g_shape_radius;
    float _shape_padding_0;
    float _shape_padding_1;
};

struct PixelOut
{
    float4 color : SV_Target0; // color
};

SamplerState g_sampler : register(s0);
Texture2D g_texture_projector : register(t0);

float4 GetPositionFromUV(float2 uv, float height, float radius)
{
    // float theta = uv.x * 3.14 * 2;
    // 
    // float x = sin(theta) * radius;
    // float z = cos(theta) * radius;
    // 
    // float y = -height * 0.5 + uv.y * height;
    // 
    // return float4(x, y, z, 1.0);
    float u4 = uv.x * 4.0;
    int face = (int) floor(u4);
    float t = frac(u4);

    // float3 p0 = float3(0, 0, radius);
    // float3 p1 = float3(radius, 0, 0);
    // float3 p2 = float3(0, 0, -radius);
    // float3 p3 = float3(-radius, 0, 0);
    float k = 0.70710678; // sqrt(2) * 0.5

    float3 p0 = float3(-radius * k, 0, radius * k);
    float3 p1 = float3(radius * k, 0, radius * k);
    float3 p2 = float3(radius * k, 0, -radius * k);
    float3 p3 = float3(-radius * k, 0, -radius * k);

    float3 posXZ;

    if (face == 0)
        posXZ = lerp(p0, p1, t);
    else if (face == 1)
        posXZ = lerp(p1, p2, t);
    else if (face == 2)
        posXZ = lerp(p2, p3, t);
    else
        posXZ = lerp(p3, p0, t);

    float y = height * 0.5 - uv.y * height;

    return float4(posXZ.x, y, posXZ.z, 1.0);
}

PixelOut main(VertexOut pixel_in) : SV_TARGET
{
    PixelOut pixel_out;
    
    float2 screen_uv = pixel_in.uv;
    float4 position_world = GetPositionFromUV(screen_uv, g_shape_height, g_shape_radius);

    float4 position_proj = mul(position_world, g_projector_matrix_view_proj);
    clip(position_proj.w - 0.001);
    position_proj /= position_proj.w;
    
    clip(position_proj.z);
    clip(1 - position_proj.z);
    
    float2 uv_proj = position_proj.xy * 0.5f + 0.5f;
    uv_proj.y = 1.0 - uv_proj.y;
    clip(uv_proj.x);
    clip(1 - uv_proj.x);
    clip(uv_proj.y);
    clip(1 - uv_proj.y);
    
    // pixel_out.color = float4(position_world.xyz, 1.0);
    // return pixel_out;
    
    //float3 to_projector = normalize(g_projector_position_w - position_world.xyz);
    //float3 normal = g_texture_normal.Sample(g_sampler, screen_uv).xyz;
    //float facing = dot(normal, to_projector);
    //clip(facing - 0.1f);

    float4 color_projector = g_texture_projector.Sample(g_sampler, uv_proj);
    clip(color_projector.a - 0.001);
    pixel_out.color = color_projector;

    return pixel_out;
}