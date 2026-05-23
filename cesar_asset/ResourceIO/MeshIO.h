#pragma once 
#include "../cesar_core/cesar_core.h"
#include "../Resource.h"
#include "ResourceIO.h"
#include "../Resources/Mesh.h"

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>

#include <filesystem>

namespace cesar
{
	struct MeshLoadDesc : public ResourceLoadDesc
	{

	};

	//struct aiNode;
	//struct aiScene;
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
		virtual void SaveToDisk(const std::filesystem::path& file_path) override;


	private:
		//Data Compression
		Vector2 OctEncode(const DirectX::XMFLOAT3& n);
		Vector3 OctDecode(const DirectX::XMFLOAT2& e);

		void OptimizeMesh(std::vector<SubMeshData>& submeshes, std::vector<Vertex>& vertices, std::vector<Uint32>& indices);
		void GenerateMeshlets(std::vector<SubMeshData>& submeshes, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<Meshlet>& meshlet, std::vector<Uint32>& meshlet_vertices, std::vector<Uint32>& meshlet_triangles);

		//Load Per Extension
		void LoadGLTF(ResourceLoadDesc& load_desc, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submesh_data, std::vector<Uint32>& mtl, std::vector<std::string>& submesh_name);

		void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submeshes, std::vector<std::string>& submesh_names, std::vector<Matrix>& model_matrix, ResourceLoadDesc& load_desc);
		void LoadWithAssimp(ResourceLoadDesc& load_desc, std::vector<Vertex>& vertices, std::vector<Uint32>& indices, std::vector<SubMeshData>& submeshes, Mesh* mesh_resource);
		

		BoundingSphere RitterSphere(const std::vector<Vertex>& vertices, size_t vertex_offset, size_t vertex_count);
		BoundingSphere MergeSpheres(BoundingSphere a, BoundingSphere b);
	};

}
