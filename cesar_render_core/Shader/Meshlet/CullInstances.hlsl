#include "../common_resources.hlsli"
struct Constants
{
    uint mesh_instances_idx;
    uint submesh_instances_idx;
    
    uint submesh_candidates_counter_idx;
    uint submesh_candidates_idx;
};

ConstantBuffer<Constants> constants : register(b1);

[numthreads(64, 1, 1)]
void CullInstances(uint3 gid : SV_GroupID, uint gtid : SV_GroupThreadID)
{
    StructuredBuffer<MeshInstance>    mesh_instances    = ResourceDescriptorHeap[constants.mesh_instances_idx];
    StructuredBuffer<SubmeshInstance> submesh_instances = ResourceDescriptorHeap[constants.submesh_instances_idx];
    
    RWByteAddressBuffer                  submesh_candidates_counter = ResourceDescriptorHeap[constants.submesh_candidates_counter_idx];
    RWStructuredBuffer<SubMeshCandidate> submesh_candidates         = ResourceDescriptorHeap[constants.submesh_candidates_idx];

    MeshInstance mesh_instance = mesh_instances[gid.x];
    
    //i is a local submesh idx. It can be used for asset submesh and instance submesh
    for (uint i = gtid; i < mesh_instance.submesh_data_count; i+=64)
    {
        const uint submesh_idx = mesh_instance.submesh_instance_start + i;
        SubmeshInstance instance = submesh_instances[submesh_idx];
        //Do Culling
        
        bool visible = true;
        if(visible)
        {
            uint index;
            submesh_candidates_counter.InterlockedAdd(0, 1, index);
            
            SubMeshCandidate candidate;
            candidate.instance_idx = gid.x;
            candidate.submesh_idx  = i;
            
            submesh_candidates[index] = candidate;
        }
    }
}