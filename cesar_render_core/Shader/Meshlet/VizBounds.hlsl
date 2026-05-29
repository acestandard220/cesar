#include "../common_resources.hlsli"

struct Constants
{
    uint submesh_candidates_idx;
    uint mesh_instances_idx;
    uint submeshes_idx;
    uint submesh_instances_idx;
    uint submesh_candidate_counter_idx;
};

ConstantBuffer<Constants> constants : register(b1);

void GetAABBVertices(float3 minBounds, float3 maxBounds, out float3 verts[8])
{
    [unroll]
    for (uint i = 0; i < 8; i++)
    {
        verts[i] = float3(
            (i & 1) ? maxBounds.x : minBounds.x,
            (i & 2) ? maxBounds.y : minBounds.y,
            (i & 4) ? maxBounds.z : minBounds.z
        );
    }
}

void GetAABBIndices(out uint3 indices[12])
{
    // Bottom face (y-)
    indices[0] = uint3(0, 1, 2);
    indices[1] = uint3(2, 1, 3);
    // Top face (y+)
    indices[2] = uint3(4, 6, 5);
    indices[3] = uint3(5, 6, 7);
    // Left face (x-)
    indices[4] = uint3(0, 2, 4);
    indices[5] = uint3(4, 2, 6);
    // Right face (x+)
    indices[6] = uint3(1, 5, 3);
    indices[7] = uint3(3, 5, 7);
    // Front face (z-)
    indices[8] = uint3(0, 4, 1);
    indices[9] = uint3(1, 4, 5);
    // Back face (z+)
    indices[10] = uint3(2, 3, 6);
    indices[11] = uint3(6, 3, 7);
}

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

[numthreads(21,1,1)]
[outputtopology("triangle")]
void VizSubMeshBounds_MS(uint3 gid : SV_GroupID, uint gtid : SV_GroupThreadID, out vertices VizVertexOut output_vertices[8 * 21],
                      out indices uint3 output_indices[12 * 21])
{
    StructuredBuffer<SubMeshCandidate> submesh_candidates        = ResourceDescriptorHeap[constants.submesh_candidates_idx];
    StructuredBuffer<MeshInstance>     mesh_instances            = ResourceDescriptorHeap[constants.mesh_instances_idx];
    StructuredBuffer<SubMeshData>      submeshes                 = ResourceDescriptorHeap[constants.submeshes_idx];
    StructuredBuffer<SubmeshInstance>  submesh_instances         = ResourceDescriptorHeap[constants.submesh_instances_idx];
    ByteAddressBuffer                  submesh_candidate_counter = ResourceDescriptorHeap[constants.submesh_candidate_counter_idx];
    
    uint thread_limit = submesh_candidate_counter.Load(0);
    
    uint group_start = gid.x * 21;
    uint active = min(21u, thread_limit > group_start ? thread_limit - group_start : 0u);
    SetMeshOutputCounts(8 * active, 12 * active);

    uint candidate_index = group_start + gtid;
    if (candidate_index >= thread_limit)
        return;
    
    SubMeshCandidate candidate        = submesh_candidates[candidate_index];
    MeshInstance     mesh_instance    = mesh_instances[candidate.instance_idx];
    SubMeshData      submesh          = submeshes[mesh_instance.submesh_data_start + candidate.submesh_idx];
    SubmeshInstance  submesh_instance = submesh_instances[mesh_instance.submesh_instance_start + candidate.submesh_idx];

    BoundingBox box = submesh.bounding_box;

    float3 local_vertices[8];
    GetAABBVertices(box.min, box.max, local_vertices);
    [unroll]
    for (uint i = 0; i < 8; i++)
        output_vertices[gtid * 8 + i] = MakeVert(local_vertices[i], submesh_instance.model_matrix, frame_constants.view_projection);
    
    uint3 box_indices[12];
    GetAABBIndices(box_indices);
    [unroll]
    for (uint i = 0; i < 12; i++)
        output_indices[gtid * 12 + i] = box_indices[i] + gtid * 8;

}


float4 VizSubMeshBounds_PS(VizVertexOut input) : SV_Target
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}