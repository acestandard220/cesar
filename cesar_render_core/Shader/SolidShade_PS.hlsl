#include "common_resources.hlsli"

struct Constants
{
    uint position_map_idx;
    uint normal_map_idx;
};

ConstantBuffer<Constants> constants : register(b1);

float4 SolidShade_PS(ScreenTriangleVertexOut input) : SV_Target
{
    Texture2D<float4> position_map = ResourceDescriptorHeap[constants.position_map_idx];
    Texture2D normal_map = ResourceDescriptorHeap[constants.normal_map_idx];
    
    int3 load_index = int3((int2) input.position.xy, 0);
    float4 pos = position_map.Load(load_index);
    float3 normal = normal_map.Load(load_index);

    if (pos.a == 0.0f)
        discard;
    
    const float3 sun_direction = float3(-1.0f, 1.0f, 1.0f);
    const float4 sun_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    const float4 base_color = float4(0.18, 0.18, 0.18, 1.0f);

    float ambient_strength = 0.1f;
    float3 ambient = ambient_strength * sun_color;
  	
    float3 norm = normalize(normal);
    float3 lightDir = normalize(sun_direction);
    float diff = max(dot(norm, lightDir), 0.0);
    float3 diffuse = sun_color * diff * base_color;
        
    float3 result = ambient + diffuse;
    return float4(result, 1.0);
}