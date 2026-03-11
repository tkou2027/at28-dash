#include "common_skybox.hlsli"

cbuffer SkyConfig : register(b0)
{
    float g_total_time;
    float _sky_padding_1;
    float _sky_padding_2;
    float _sky_padding_3;
};

// ============================================
// --- 3D Noise  ---
// ============================================
float hash3(float3 p)
{
    return -1.0 + 2.0 *frac(sin(dot(p, float3(12.9898, 78.233, 37.719))) * 43758.5453);
}

float noise3(float3 x)
{
    float3 i = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);

    return lerp(
        lerp(lerp(hash3(i + float3(0, 0, 0)), hash3(i + float3(1, 0, 0)), f.x),
             lerp(hash3(i + float3(0, 1, 0)), hash3(i + float3(1, 1, 0)), f.x), f.y),
        lerp(lerp(hash3(i + float3(0, 0, 1)), hash3(i + float3(1, 0, 1)), f.x),
             lerp(hash3(i + float3(0, 1, 1)), hash3(i + float3(1, 1, 1)), f.x), f.y), f.z);
}

// 3. 3D FBM (分形布朗運動)
float FBM3(float3 p)
{
    float f = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < 2; i++)
    {
        f += amp * noise3(p * freq);
        freq *= 2.0;
        amp *= 0.5;
    }
    return f;
}

// --- band ---
float3 GetFluidColor(float t)
{
    float3 c1 = float3(0.5, 0.0, 0.7); // 
    float3 c2 = float3(0.0, 0.1, 0.8); // 
    float3 c3 = float3(0.2, 0.2, 0.6); // 
    float3 c_black = float3(0.0, 0.0, 0.0); // 
    float3 c5 = float3(0.0, 0.0, 1.0); // 
    
    float3 col = lerp(c1, c2, smoothstep(0.0, 0.3, t));
    col = lerp(col, c3, smoothstep(0.3, 0.6, t));
    col = lerp(col, c_black, smoothstep(0.65, 0.68, t));
    col = lerp(col, c5, smoothstep(0.72, 0.75, t));
    
    return col;
}


float4 main(VertexOutSkybox pixelIn) : SV_Target
{
    float3 viewDir = normalize(pixelIn.posL);
    
    
    float3 p = viewDir * 4.0f;
    
    
    float warp_time = g_total_time * 0.03;
    
    float3 q;
    q.x = FBM3(p + warp_time);
    q.y = FBM3(p + float3(5.2, 1.3, 8.4) - warp_time);
    q.z = FBM3(p + float3(2.1, 7.8, 3.5)); 

    float3 r_warp = q;
    r_warp.x = FBM3(p + 4.0 * q + warp_time * 1.5);
    r_warp.y = FBM3(p + 4.0 * q + float3(9.2, 2.8, 1.1));
    r_warp.z = FBM3(p + 4.0 * q + float3(4.5, 6.1, 8.9));

   
    float f = saturate(0.5 + 0.5 * FBM3(p + 4.0 * r_warp));

    
    float3 fluidColor = GetFluidColor(f);
    
    float3 compositeColor = saturate(fluidColor );
    
    return float4(compositeColor, 1.0);
}