#include "../common_resources.hlsli"

struct Constants
{
    uint depth_map_idx;
    uint active_clusters_idx;

    uint tile_size;
    uint tile_count_x;
    uint tile_count_y;
    
    uint depth_slice_scale;
    uint depth_slice_bias;
};

ConstantBuffer<Constants> constants;

float LinearizeDepth(float d, float near_plane, float far_plane)
{
    return near_plane * far_plane / (far_plane - d * (far_plane - near_plane));
}

uint GetClusterIndex(float2 screen_position, float linear_depth)
{
    uint depth_slice = uint(max(log2(linear_depth) * constants.depth_slice_scale + constants.depth_slice_bias, 0.0f));

    uint3 cluster = uint3(screen_position / constants.tile_size, depth_slice);
    uint cluster_index = cluster.x +
                         cluster.y * constants.tile_count_x +
                         cluster.z * (constants.tile_count_x * constants.tile_count_y);

    return cluster_index;
}

[numthreads(32, 32, 1)]
void CullClusters(uint3 gtid :SV_DispatchThreadID)
{
    Texture2D<float> depth_map = ResourceDescriptorHeap[constants.depth_map_idx];
    RWStructuredBuffer<uint> active_clusters = ResourceDescriptorHeap[constants.active_clusters_idx];
    
    if (gtid.x >= frame_constants.screen_width || gtid.y >= frame_constants.screen_height)
        return;
    
    float2 screen_coord = gtid.xy / float2(frame_constants.screen_width, frame_constants.screen_height);
    float depth_value = depth_map.Sample(linear_clamp, screen_coord); //Check
    float linearized_depth = LinearizeDepth(depth_value, frame_constants.camera_near, frame_constants.camera_far);
    
    if (depth_value >= 1.0f)
        return; 
    
    uint cluster_index = GetClusterIndex(float2(gtid.xy), linearized_depth);

    active_clusters[cluster_index] = true;
}



