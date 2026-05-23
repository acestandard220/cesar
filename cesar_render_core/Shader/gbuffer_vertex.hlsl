#include "common_resources.hlsli"

VertexOut VertexMain(VertexIn input)
{
    VertexOut output;
    output.position = mul(frame_constants.view_projection, float4(input.position, 1.0));
    output.normal = float3(0.0, 1.0, 0.0);
    output.tangent = float4(1, 0, 1, 1);
    output.uv = float2(1, 0);
    return output;
}