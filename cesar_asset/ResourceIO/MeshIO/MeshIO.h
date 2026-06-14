#pragma once 
#include "../../../cesar_core/cesar_core.h"
#include "../../Resource.h"
#include "../ResourceIO.h"
#include "../../Resources/Mesh.h"

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include <filesystem>

namespace cesar
{
	//Todo: If *Type*ResourceLoadDesc only end up storing load flags and other data that is also used in some way by other 
	//loaders, move it to resource load desc as a Uint32 or Int32
	enum class MeshLoadFlags
	{
		None = BIT(0),
		LoadMeshMaterials = BIT(1)
	};
	CESAR_ENABLE_ENUM_OPS(MeshLoadFlags);

	struct MeshLoadDesc : public ResourceLoadDesc
	{
		MeshLoadFlags load_flags;
	};

	// | Vertices | Indices | SubmeshDatas | Meshlets | MeshletTriangles | MeshletVertices | ModelMatrix  
	struct MeshAssetHeader : public CesarAssetHeader
	{
		//These are the .casset file fields not the global buffer fields
		Uint64 vertex_count;
		Uint64 vertex_start;

		Uint64 index_count;
		Uint64 index_start;

		Uint64 submesh_start;
		Uint64 submesh_count;

		Uint64 meshlet_start;
		Uint64 meshlet_count;

		Uint64 meshlet_vertex_start;
		Uint64 meshlet_vertex_count;

		Uint64 meshlet_triangle_start;
		Uint64 meshlet_triangle_count;

		Uint64 submesh_names_start;
		Uint64 submesh_matrixes_start;
		Uint64 submesh_material_start;

		Matrix model_matrix;
	};

	class MeshIO :public IResourceIO
	{
		struct SubmeshHolder {
			Uint32 vertex_start;
			Uint32 index_start;

			Uint32 vertex_count;
			Uint32 index_count;
		};
	public:
		MeshIO() = default;
		MeshIO(ResourceCache* resource_cache)
			:IResourceIO(resource_cache)
		{

		}
		virtual ~MeshIO() = default;

		virtual std::unique_ptr<Resource> LoadFromFile(ResourceLoadDesc& load_desc) override;
		virtual void SaveToDisk(const ResourceLoadDesc& load_desc, void* mesh_resource) override;


	private:
		void OptimizeMesh(std::vector<SubMeshData>& submeshes, std::vector<Vertex>& vertices, std::vector<Uint32>& indices);
		void GenerateMeshlets(std::vector<SubMeshData>& submeshes, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<Meshlet>& meshlet, std::vector<Uint32>& meshlet_vertices, std::vector<Uint32>& meshlet_triangles);

		void LoadFastGLTF(ResourceLoadDesc& load_desc, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submesh_data, Mesh* mesh);

		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submeshes, std::vector<std::string>& submesh_names, std::vector<Matrix>& model_matrix, ResourceLoadDesc& load_desc);
		void LoadWithAssimp(ResourceLoadDesc& load_desc, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submeshes, Mesh* mesh_resource);

		BoundingSphere RitterSphere(const std::vector<Vertex>& vertices, size_t vertex_offset, size_t vertex_count);
		BoundingSphere MergeSpheres(BoundingSphere a, BoundingSphere b);
	};

}
