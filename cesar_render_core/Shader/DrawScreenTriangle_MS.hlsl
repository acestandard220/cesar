#include "common_resources.hlsli"

[numthreads(1, 1, 1)]
[outputtopology("triangle")]
void DrawScreenTriangle_MS(out vertices ScreenTriangleVertexOut verts[3], out indices uint3 indices[1])
{
    const float2 v[3] =
    {
        float2(-1.0f, -1.0f),
        float2(-1.0f, 3.0f),
        float2(3.0f, -1.0f)
    };

 
    SetMeshOutputCounts(3, 1);
    
    for (uint i = 0; i < 3; i++)
    {
        ScreenTriangleVertexOut vo;
        vo.position = float4(v[i], 0.0f, 1.0f);
        vo.uv = v[i] * 0.5 + 0.5f;

        verts[i] = vo;
    }
    
    indices[0] = uint3(0, 1, 2);
}