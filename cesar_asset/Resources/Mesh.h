#pragma once
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"

#include <SimpleMath/SimpleMath.h>
using namespace DirectX::SimpleMath;

namespace cesar {
	static constexpr Uint32 ONE = 1;

	static constexpr Uint32 START_MESH_COUNT = ONE;
	static constexpr Uint32 START_INSTANCE_PER_MESH = ONE;

	static constexpr Uint32 START_MESHLETS_COUNT = ONE;
    //static constexpr Uint32 MAX_INSTANCES_COUNT  = START_MESH_COUNT * MAX_INSTANCE_PER_MESH;

	static constexpr Uint32 MAX_MESHLET_VERTICE  = 64;
	static constexpr Uint32 MAX_MESHLET_TRIANGLE = 124;

	static constexpr Uint32 START_VERTEX_COUNT = START_MESHLETS_COUNT * MAX_MESHLET_VERTICE;
	static constexpr Uint32 START_INDEX_COUNT = START_MESHLETS_COUNT * MAX_MESHLET_TRIANGLE;
	static constexpr Uint32 START_SUBMESH_COUNT = ONE;

	struct BoundingBox {
		Vector3 min = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		Vector3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	};

	struct BoundingSphere {
		Vector3 center = { 0.0f,0.0f,0.0f };
		Float radius = 0.0f;
	};

	struct Vertex {
		
		Vector3 position;
		Vector3 normal;
		Vector4 tangent;
		Vector2 uv;
	};

	struct MeshletCandidate {
		Uint32 meshlet_idx;
		Uint32 submesh_idx;
	};

	struct Meshlet {
		Vector3 center;
		Float   radius;

		Uint32 vertex_start;
		Uint32 triangle_start;

		Uint32 vertex_count;
		Uint32 triangle_count;
	};

	/// <summary>
	/// This class represents a single submesh/primitive of a glb load.
	/// </summary>
	struct SubMeshData
	{
		Uint32 vertex_start;
		Uint32 vertex_count;
		Uint32 index_start;
		Uint32 index_count;

		Uint32 meshlet_start;
		Uint32 meshlet_count;
		Uint32 meshlet_vertice_start;
		Uint32 meshlet_triangle_start;

		BoundingBox bounding_box{};
		Uint32 pad02[2];
	};

	struct Mesh : public Resource
	{
		Uint32 submesh_start      = 0;
		Uint32 submesh_data_count = 0;

		Uint32 meshlet_start = 0;
		Uint32 meshlet_count = 0;

		Matrix model_matrix;

		std::vector<Matrix> model_matrixes;
		std::vector<std::string> submesh_names;
		std::vector<Uint32> default_materials; //Per-submesh //Indexed with submesh_id
	};

	template<> struct ResourceTypeTrait<Mesh> { static constexpr ResourceType type = ResourceType::Mesh; };
}