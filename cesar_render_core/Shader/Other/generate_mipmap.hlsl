#include "../common_resources.hlsli"

struct Constants
{
    uint src_texture_idx;
    uint dst_texture_idx;
    
    float2 TexelSize; // 1.0 / dst dimensions
};

ConstantBuffer<Constants> constants : register(b1);

[numthreads(8, 8, 1)]
void GenerateMipMap(uint3 DTid : SV_DispatchThreadID)
{
    Texture2D<float4>   src_texture = ResourceDescriptorHeap[constants.src_texture_idx];
    RWTexture2D<float4> dst_texture = ResourceDescriptorHeap[constants.dst_texture_idx];
    
    // Sample center of the 4 source texels that map to this dst texel
    float2 uv = (DTid.xy + 0.5f) * constants.TexelSize;
    dst_texture[DTid.xy] = src_texture.SampleLevel(linear_clamp, uv, 0);
}