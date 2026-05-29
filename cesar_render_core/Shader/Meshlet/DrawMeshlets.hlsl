#include "../common_resources.hlsli"

#define MAX_VERTICES 64
#define MAX_PRIMITIVES 124

#define THREADS_X 32
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
    uint submesh_candidates_idx;
    uint mesh_instances_idx;
    uint submesh_instances_idx;
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

VertexOut GetVertex(Meshlet meshlet, SubMeshData submesh, SubmeshInstance submesh_instance, uint i, uint id)
{
    StructuredBuffer<Vertex>          vertices         = ResourceDescriptorHeap[constant.vertices_idx];
    StructuredBuffer<uint>            meshlet_vertices = ResourceDescriptorHeap[constant.meshlet_vertices_idx];
    
    Vertex v = vertices[submesh.vertex_start + meshlet_vertices[submesh.meshlet_vertice_start + meshlet.vertex_start + i]];

    VertexOut v_out;
    float4 position = mul(float4(v.position, 1.0f), submesh_instance.model_matrix);
    v_out.position = mul(frame_constants.view_projection, position);
    v_out.normal = GetMeshletColor(id);
    return v_out;
}

[numthreads(THREADS_X, THREADS_Y, THREADS_Z)]
[outputtopology("triangle")]
void DrawMeshlet(uint3 gid : SV_GroupID, uint gtid : SV_GroupThreadID, out vertices VertexOut output_vertices[MAX_VERTICES], out indices uint3 output_indices[MAX_PRIMITIVES])
{
    StructuredBuffer<Meshlet>          meshlets           = ResourceDescriptorHeap[constant.meshlets_idx];
    StructuredBuffer<MeshletCandidate> meshlet_candidate  = ResourceDescriptorHeap[constant.meshlet_candidates_idx];
    StructuredBuffer<SubMeshData>      submeshes          = ResourceDescriptorHeap[constant.submeshes_idx];
    StructuredBuffer<SubMeshCandidate> submesh_candidates = ResourceDescriptorHeap[constant.submesh_candidates_idx];
    StructuredBuffer<MeshInstance>     mesh_instances     = ResourceDescriptorHeap[constant.mesh_instances_idx];
    StructuredBuffer<SubmeshInstance>  submesh_instances  = ResourceDescriptorHeap[constant.submesh_instances_idx];
    
    MeshletCandidate candidate = meshlet_candidate[gid.x];
    SubMeshCandidate submesh_candidate = submesh_candidates[candidate.submesh_candidate_idx];

    Meshlet          meshlet   = meshlets[candidate.meshlet_idx]; 
    MeshInstance mesh_instance = mesh_instances[submesh_candidate.instance_idx];

    SubMeshData submesh = submeshes[mesh_instance.submesh_data_start + submesh_candidate.submesh_idx];
    SubmeshInstance submesh_instance = submesh_instances[mesh_instance.submesh_instance_start + submesh_candidate.submesh_idx];
    
    SetMeshOutputCounts(meshlet.vertex_count, meshlet.triangle_count);
    
    for (int i = gtid; i < meshlet.vertex_count; i += THREADS_X)
    {    
        output_vertices[i] = GetVertex(meshlet, submesh, submesh_instance, i, candidate.meshlet_idx);
    }
    
    for (int j = gtid; j < meshlet.triangle_count; j += THREADS_X)
    {
        output_indices[j] = GetTriangle(meshlet, submesh, j);
    }
}