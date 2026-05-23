#include "common_resources.hlsli"

struct GBufferOutputs
{
    float4 position_map : SV_Target0;
    float4 normal_map   : SV_Target1;
    float4 arm_map      : SV_Target2;
    float4 albedo_map   : SV_Target3;
};

GBufferOutputs PixelMain(VertexOut input)
{
    GBufferOutputs output;

    output.position_map = float4(input.normal, 1.0f);
    output.normal_map = float4(1, 1, 1, 1);
    output.arm_map = float4(1, 1, 1, 1);
    output.albedo_map = float4(0.8f, 0.8f, 0.8f, 1);

    return output;
}