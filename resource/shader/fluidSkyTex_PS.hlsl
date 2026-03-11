Texture2D g_NoiseTex : register(t0);
SamplerState g_SamLinearWrap : register(s5);

// --- Precise constants for a 4096x4096 (16x16 Tiles) atlas ---
static const float NUM_CELLS = 16.0;       // The atlas is a 16x16 grid
static const float TOTAL_SLICES = 256.0;   // Total number of Z-slices (16 * 16 = 256)
static const float TILE_PIXELS = 256.0;    // Resolution of a single tile (4096 / 16 = 256)

cbuffer SkyConfig : register(b0)
{
    float g_total_time;
    float _sky_padding_1;
    float _sky_padding_2;
    float _sky_padding_3;
};

float SampleNoise3D(float3 p)
{
    // 1. Noise scale adjustment
    float noiseScale = 0.2; 
    float3 uvw = frac(p * noiseScale);
    
    // 2. Z-axis calculation: Map to the 0 ~ 255 slice range.
    float z = uvw.z * TOTAL_SLICES;
    float z0 = floor(z);
    float z1 = z0 + 1.0;
    float zWeight = frac(z);
    
    // Ensure seamless wrapping along the Z-axis (0~255).
    z0 = fmod(z0, TOTAL_SLICES);
    z1 = fmod(z1, TOTAL_SLICES);
    
    // Calculate the 2D Tile coordinates (X, Y) within the 16x16 grid.
    float2 tile0 = float2(fmod(z0, NUM_CELLS), floor(z0 / NUM_CELLS));
    float2 tile1 = float2(fmod(z1, NUM_CELLS), floor(z1 / NUM_CELLS));

    // 3. Bleeding prevention
    // The half-pixel size must be based on the single tile resolution (256).
    float halfPixel = 0.5 / TILE_PIXELS;
    float2 safe_uv = clamp(uvw.xy, halfPixel, 1.0 - halfPixel);
    
    // Add the tile offset to the local UVs and scale down to the 0~1 space of the atlas.
    float2 uv0 = (tile0 + safe_uv) / NUM_CELLS;
    float2 uv1 = (tile1 + safe_uv) / NUM_CELLS;

    // Sample the two adjacent Z-slices.
    float noise0 = g_NoiseTex.SampleLevel(g_SamLinearWrap, uv0, 0).r;
    float noise1 = g_NoiseTex.SampleLevel(g_SamLinearWrap, uv1, 0).r;
    
    // Interpolate along the Z-axis and remap from [0, 1] to [-1, 1].
    // Returning values from -1 to 1 is critical to match the behavior of the original procedural noise3().
    return lerp(noise0, noise1, zWeight) * 2.0 - 1.0;
}

// --- Fractional Brownian Motion (FBM) ---
float FBM3(float3 p, uint sampleTime)
{
    float f = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    

    [unroll]
    for (int i = 0; i < sampleTime; i++)
    {
        f += amp * SampleNoise3D(p * freq);
        freq *= 2.0;
        amp *= 0.5;
    }
    return f;
}

// --- Color Gradient Mapping ---
float3 GetFluidColor(float t)
{
    float3 c1 = float3(0.5, 0.6, 0.7); // Purple
    float3 c2 = float3(0.0, 0.1, 0.8); // Deep Blue
    float3 c3 = float3(0.2, 0.2, 0.6); // Muted Blue
    float3 c_black = float3(0.0, 0.0, 0.0);
    float3 c5 = float3(0.0, 0.0, 1.0); // Bright Blue
    
    float3 col = lerp(c1, c2, smoothstep(0.0, 0.3, t));
    col = lerp(col, c3, smoothstep(0.3, 0.6, t));
    col = lerp(col, c_black, smoothstep(0.65, 0.68, t));
    col = lerp(col, c5, smoothstep(0.72, 0.75, t));
    
    return col;
}

float4 main(float3 worldPos : Position) : SV_Target
{
    float3 viewDir = normalize(worldPos);
    
    
    float3 p = viewDir * 1.5f;
    
    // Animation time factor
    float warp_time = g_total_time * 0.1;
    
    float3 q;
    q.x = FBM3(p + warp_time, 2);
    q.y = FBM3(p + float3(5.2, 1.3, 8.4) - warp_time, 2);
    q.z = FBM3(p + float3(2.1, 7.8, 3.5), 2);

    float3 r_warp;
    r_warp.x = FBM3(p + 4.0 * q + warp_time * 1.5, 2);
    r_warp.y = FBM3(p + 4.0 * q + float3(9.2, 2.8, 1.1), 2);
    r_warp.z = FBM3(p + 4.0 * q + float3(4.5, 6.1, 8.9), 2);

    float f = saturate(0.5 + 0.5 * FBM3(p + 4.0 * r_warp, 2));


    return float4(GetFluidColor(f), 1.0);
}