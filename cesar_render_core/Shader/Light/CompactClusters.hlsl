#include "../common_resources.hlsli"

struct Constants
{
    uint active_clusters_idx;
    uint compacted_clusters_idx;
    uint active_clusters_counter_idx;

    uint tile_count_x;
    uint tile_count_y;
    uint tile_count_z;
};

ConstantBuffer<Constants> constants : register(b1);

[numthreads(32, 1, 1)]
void CompactClusters(uint3 gtid : SV_DispatchThreadID)
{
    StructuredBuffer<uint>   active_clusters           = ResourceDescriptorHeap[constants.active_clusters_idx];
    RWStructuredBuffer<uint> compacted_active_clusters = ResourceDescriptorHeap[constants.compacted_clusters_idx];
    RWByteAddressBuffer      active_clusters_counter   = ResourceDescriptorHeap[constants.active_clusters_counter_idx];
    
    uint thread_id = gtid.x;
    const uint thread_limit = constants.tile_count_x * constants.tile_count_y * constants.tile_count_z;
    if(thread_id >= thread_limit)
        return;

    
    if (active_clusters[thread_id])
    {
        uint idx;
        active_clusters_counter.InterlockedAdd(0, 1, idx);
        compacted_active_clusters[idx] = thread_id;
    }
}