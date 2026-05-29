struct VizVertexOut
{
    float4 position : SV_Position;
};

VizVertexOut MakeVert(float3 pos, float4x4 model, float4x4 vp)
{
    VizVertexOut v;
    v.position = mul(vp, mul(float4(pos, 1.0f), model));
    return v;
}