#include "meshlet_common.hlsli"
#include "../common_resources.hlsli"

#define MAX_VERTICES 64
#define MAX_PRIMITIVES 124

#define THREADS_X 128
#define THREADS_Y 1
#define THREADS_Z 1

struct Constants
{
    uint meshlets_idx;
    uint vertices_idx;
    uint meshlet_vertices_idx;
    uint meshlet_triangles_idx;
    
    uint meshlet_candidates_idx;
    uint submeshes_idx;
};

ConstantBuffer<Constants> constant : register(b1);

uint3 GetTriangle(Meshlet meshlet, SubMeshData submesh, uint i)
{
    StructuredBuffer<uint> meshlet_triangles = ResourceDescriptorHeap[constant.meshlet_triangles_idx];
        
    uint3 t_out;
    t_out.x = meshlet_triangles[submesh.meshlet_triangle_start + meshlet.triangle_start + i * 3 + 0];
    t_out.y = meshlet_triangles[submesh.meshlet_triangle_start + meshlet.triangle_start + i * 3 + 1];
    t_out.z = meshlet_triangles[submesh.meshlet_triangle_start + meshlet.triangle_start + i * 3 + 2];
    return t_out;
}

VertexOut GetVertex(Meshlet meshlet, SubMeshData submesh, uint i,uint id)
{
    StructuredBuffer<Vertex>          vertices         = ResourceDescriptorHeap[constant.vertices_idx];
    StructuredBuffer<uint>            meshlet_vertices = ResourceDescriptorHeap[constant.meshlet_vertices_idx];
    
    Vertex v = vertices[submesh.vertex_start + meshlet_vertices[submesh.meshlet_vertice_start + meshlet.vertex_start + i]];

    VertexOut v_out;
    v_out.position = mul(frame_constants.view_projection, float4(v.position, 1.0f));
    v_out.normal = GetMeshletColor(id);
    return v_out;
}

[numthreads(THREADS_X, THREADS_Y, THREADS_Z)]
[outputtopology("triangle")]
void DrawMeshlet(uint3 gid : SV_GroupID, uint gtid : SV_GroupThreadID, out vertices VertexOut output_vertices[MAX_VERTICES], out indices uint3 output_indices[MAX_PRIMITIVES])
{
    StructuredBuffer<Meshlet>          meshlets          = ResourceDescriptorHeap[constant.meshlets_idx];
    StructuredBuffer<MeshletCandidate> meshlet_candidate = ResourceDescriptorHeap[constant.meshlet_candidates_idx];
    StructuredBuffer<SubMeshData>      submeshes         = ResourceDescriptorHeap[constant.submeshes_idx];
    
    MeshletCandidate candidate = meshlet_candidate[gid.x];
    Meshlet          meshlet   = meshlets[candidate.meshlet_idx]; 
    SubMeshData     submesh    = submeshes[candidate.submesh_idx];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);
    
    for (int i = gtid; i < meshlet.vertex_count; i += THREADS_X)
    {    
        output_vertices[i] = GetVertex(meshlet, submesh, i, candidate.meshlet_idx);
    }
    
    for (int j = gtid; j < meshlet.triangle_count; j += THREADS_X)
    {
        output_indices[j] = GetTriangle(meshlet, submesh, j);
    }
}