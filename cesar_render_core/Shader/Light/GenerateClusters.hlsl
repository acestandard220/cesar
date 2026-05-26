// Reference: https://mmzala.github.io/blog/clustered-shading.html

#include "../common_resources.hlsli" //Move things out of common
#include "LightCommon.hlsli"

struct Constants
{
    uint clusters_idx;
    
    uint tile_x;
    uint tile_y;
    uint tile_z;
};

ConstantBuffer<Constants> constants : register(b1);

float3 ScreenSpaceToViewSpace(float3 position)
{
    float4 transformed = mul(frame_constants.inverse_projection, float4(position, 1.0f));
    return transformed.xyz;
}

float3 IntersectLineToZPlane(float3 p0, float3 p1, float z)
{
    float t = (z - p0.z) / (p1.z - p0.z);
    return lerp(p0, p1, t);
}

[numthreads(16, 16, 1)]
void GenerateClusters(uint3 tid : SV_DispatchThreadID, uint3 gtid : SV_GroupThreadID, uint gid : SV_GroupID)
{
    if (tid.x >= constants.tile_x || tid.y >= constants.tile_y || tid.z >= constants.tile_z)
        return;
    
    RWStructuredBuffer<Cluster> clusters = ResourceDescriptorHeap[constants.clusters_idx];
    
    uint cluster_index = tid.x + tid.y * constants.tile_x + tid.z * constants.tile_x * constants.tile_y;
    
    float2 tile_min = float2(tid.xy) * TILE_SIZE;
    float2 tile_max = float2(tid.xy + 1) * TILE_SIZE;

    float3 cluster_min_ss = float3((tile_min / float2(frame_constants.screen_width, frame_constants.screen_height)) * 2.0f - 1.0f, 1.0f);
    float3 cluster_max_ss = float3((tile_max / float2(frame_constants.screen_width, frame_constants.screen_height)) * 2.0f - 1.0f, 1.0f);
        
    float3 cluster_min_vs = ScreenSpaceToViewSpace(cluster_min_ss);
    float3 cluster_max_vs = ScreenSpaceToViewSpace(cluster_max_ss);

    
    float plane_near = -frame_constants.camera_near * pow(frame_constants.camera_far / frame_constants.camera_near, float(tid.z)     / constants.tile_z);
    float plane_far  = -frame_constants.camera_near * pow(frame_constants.camera_far / frame_constants.camera_near, float(tid.z + 1) / constants.tile_z);
    
    const float3 eye_position = float3(0.0f, 0.0f, 0.0f);
    
    float3 min_point_near = IntersectLineToZPlane(eye_position, cluster_min_vs, plane_near);
    float3 min_point_far  = IntersectLineToZPlane(eye_position, cluster_min_vs, plane_far);

    float3 max_point_near = IntersectLineToZPlane(eye_position, cluster_max_vs, plane_near);
    float3 max_point_far  = IntersectLineToZPlane(eye_position, cluster_max_vs, plane_far);
    
    clusters[cluster_index].min = min(min(min_point_near, min_point_far), min(max_point_near, max_point_far));
    clusters[cluster_index].max = max(max(min_point_near, min_point_far), max(max_point_near, max_point_far));
}