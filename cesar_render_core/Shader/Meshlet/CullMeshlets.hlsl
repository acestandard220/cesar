#include "../common_resources.hlsli"

struct Constants
{
    uint submesh_candidates_idx;
    uint mesh_instances_idx;

    uint submeshes_idx;
    uint meshlet_idx;
    
    uint meshlet_candidates_idx;
    uint meshlet_candidates_counter_idx;
};

ConstantBuffer<Constants> constants : register(b1);

[numthreads(64, 1, 1)]
void CullMeshlets(uint3 gid : SV_GroupID, uint gtid : SV_GroupThreadID)
{
    StructuredBuffer<SubMeshCandidate> submesh_candidates = ResourceDescriptorHeap[constants.submesh_candidates_idx];
    StructuredBuffer<MeshInstance>     mesh_instances     = ResourceDescriptorHeap[constants.mesh_instances_idx];
    StructuredBuffer<SubMeshData>      submeshes          = ResourceDescriptorHeap[constants.submeshes_idx];
    StructuredBuffer<Meshlet>          meshlets           = ResourceDescriptorHeap[constants.meshlet_idx];
    
    RWStructuredBuffer<MeshletCandidate> meshlet_candidate         = ResourceDescriptorHeap[constants.meshlet_candidates_idx];
    RWByteAddressBuffer                  meshelt_candidate_counter = ResourceDescriptorHeap[constants.meshlet_candidates_counter_idx];
    
    SubMeshCandidate candidate     = submesh_candidates[gid.x];
    MeshInstance     mesh_instance = mesh_instances[candidate.instance_idx];
    
    uint        submesh_idx  = mesh_instance.submesh_data_start + candidate.submesh_idx;
    SubMeshData submesh      = submeshes[submesh_idx];
    
    for (uint i = gtid; i < submesh.meshlet_count; i += 64)
    {
        Meshlet meshlet = meshlets[submesh.meshlet_start + i];
        bool visible = true;
        if(visible)
        {
            uint index;
            meshelt_candidate_counter.InterlockedAdd(0, 1, index);

            MeshletCandidate candidate;
            candidate.meshlet_idx = submesh.meshlet_start + i;
            candidate.submesh_candidate_idx = gid.x;
            
            meshlet_candidate[index] = candidate;
        }
    }
}




struct BuildConstants
{
    uint dispatch_arguments_idx;
    uint submesh_candidate_counter_idx;
};

ConstantBuffer<BuildConstants> build_constants : register(b1);

[numthreads(1, 1, 1)]
void BuildCullMeshletArgs(uint3 gid : SV_GroupID, uint gtid : SV_GroupThreadID)
{
    RWStructuredBuffer<DispatchArguments> dispatch_arguments = ResourceDescriptorHeap[build_constants.dispatch_arguments_idx];
    ByteAddressBuffer submesh_candidate_counter              = ResourceDescriptorHeap[build_constants.submesh_candidate_counter_idx];
    
    DispatchArguments args;
    args.threads_x = submesh_candidate_counter.Load(0);
    args.threads_y = 1;
    args.threads_z = 1;
    
    dispatch_arguments[0] = args;
}