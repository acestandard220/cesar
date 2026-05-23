#define AS_THREADS_X 32

struct MeshletPayload
{
    uint active_meshlets[AS_THREADS_X];
    uint meshlet_submesh_candidate[AS_THREADS_X];
};

