
struct FrameConstants
{
    float4x4 view;
    float4x4 projection;
    float4x4 view_projection;
    
    float4 camera_position;
    float camera_fov;
    float camera_near;
    float camera_far;
    
    float p[9];
};

ConstantBuffer<FrameConstants> frame_constants : register(b0);

SamplerState linear_clamp : register(s0);

struct DispatchArguments
{
    uint threads_x;
    uint threads_y;
    uint threads_z;
};
struct Vertex
{
    float3 position;
    float3 normal;
    float4 tangent;
    float2 uv;
};

struct BoundingBox
{
    float3 min;
    float3 max;
};

struct BoundingSphere
{
    float3 center;
    float  radius;
};

struct SubmeshInstance
{
    float4x4 model_matrix;
    uint     material_index;
};

struct MeshInstance
{
    float4x4 model_matrix;
    uint     submesh_data_start;
    uint     submesh_instance_start;
    uint     submesh_data_count;
};

struct MeshletCandidate
{
    uint meshlet_idx;
    uint submesh_candidate_idx;
};
struct SubMeshCandidate
{
    uint instance_idx;
    uint submesh_idx;
};

struct Meshlet
{
    float3 center;
    float radius;

    uint vertex_start;
    uint triangle_start;

    uint vertex_count;
    uint triangle_count;
};

struct SubMeshData
{
    uint vertex_start;
    uint vertex_count;
    uint index_start;
    uint index_count;

    uint meshlet_start;
    uint meshlet_count;
    uint meshlet_vertice_start;
    uint meshlet_triangle_start;

    BoundingBox bounding_box;
    uint pad02[2];
};


struct VertexIn
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

struct VertexOut
{
    float4 position : SV_Position;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 uv : TEXCOORD;
};


uint hash(uint x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}

float3 GetMeshletColor(uint id)
{
    uint h = hash(id);
    return float3(
        (h & 0xFF) / 255.0f,
        ((h >> 8) & 0xFF) / 255.0f,
        ((h >> 16) & 0xFF) / 255.0f
    );
}