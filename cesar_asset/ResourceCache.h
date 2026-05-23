#pragma once
#include "../cesar_core/cesar_core.h"
#include "../cesar_core/Allocator/LinearAllocator.h"

#include "../cesar_render_core/OfflineContext.h"

#include "Resource.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/ImageTexture.h"

#include "ResourceIO/MeshIO.h"
#include "ResourceIO/MaterialIO.h"
#include "ResourceIO/ResourceIO.h"
#include "ResourceIO/ImageTextureIO.h"


#include <Tracy/tracy/Tracy.hpp>

namespace cesar
{

	class __declspec(dllexport) ResourceCache 
	{
		public:
			ResourceCache(OfflineContext* offline_context);
			~ResourceCache() = default;

			UUID IsCached(const std::filesystem::path& path)const {
				auto it = path_uuid_map.find(path);
				return (it != path_uuid_map.end()) ? it->second : CESAR_INVALID_UUID;
			}

			template<IsResourceType T>
			T* GetResource(UUID uuid) {
				ZoneScopedN("ResourceCache::GetResource");

				ResourceType type = ResourceTypeTrait<T>::type;
				Resource* resource = resources[static_cast<Uint32>(type)][uuid].get();
				if (resource == nullptr) {
					LOG_ERROR("Resource has not been loaded.");
					return nullptr;
				}
				return (T*)resource;
			}

			ImageTexture* GetImageTexture(UUID uuid) { return GetResource<ImageTexture>(uuid); }
			Mesh* GetMesh(UUID uuid) { return GetResource<Mesh>(uuid); }
			Material* GetMaterial(UUID uuid) { return GetResource<Material>(uuid); }

			template<typename T>
			std::vector<T*> GetResource() {
				ResourceType type = ResourceTypeTrait<T>::type;

				auto& resource_of_type = resources[static_cast<Uint32>(type)];

				std::vector<T*> return_resources(resource_of_type.size());
				Uint32 i = 0;
				for (auto& [uuid, resource] : resource_of_type) {
					return_resources[i] = static_cast<T*>(resource.get());
					i++;
				}

				return return_resources;
			}

			template<IsResourceType T>
			T* LoadResource(ResourceLoadDesc& load_desc) {
				ZoneScopedN("ResourceCache::LoadResource");

				UUID uuid = IsCached(load_desc.file_path);

				if (uuid) {
					LOG_WARN("Resource has already been loaded.");
					return static_cast<T*>(resources[static_cast<Uint32>(load_desc.type)][uuid].get());
				}

				if (!std::filesystem::exists(load_desc.file_path) && !load_desc.no_path)
				{
					LOG_ERROR("Specified path does not exist.");
					return nullptr;
				}

				IResourceIO* io = GetResourceIO(load_desc.type);
				std::unique_ptr<Resource> resource = io->LoadFromFile(load_desc);
				if (!resource) {
					LOG_ERROR("Failed to load resource.");
					return nullptr;
				}

				LOG_DEBUG("Resource loaded successfully");
				return static_cast<T*>(Cache(resource, load_desc));
			}

			ImageTexture* GetDefaultInvalidTexture() const;
			ImageTexture* GetDefaultWhiteTexture() const;
			ImageTexture* GetDefaultBlackTexture() const;
			ImageTexture* GetDefaultNormalTexture() const;

			LinearAllocator<MaterialData>* GetMaterialAllocator() { return material_data.get(); }

			MemoryBlock<SubMeshData> GetSubMeshDataBlock(Uint32 index, Uint32 count) { return submesh_data->GetMemoryBlock(index, count); }

			LinearAllocator<Vertex>*  GetVertexAllocator()  { return vertices.get(); }
			LinearAllocator<Uint32>*  GetIndexAllocator()   { return indices.get(); }
			LinearAllocator<SubMeshData>* GetMeshAllocator() { return submesh_data.get(); }

			LinearAllocator<Meshlet>* GetMeshletAllocator()         { return meshlet_data.get(); }
			LinearAllocator<Uint32>*  GetMeshletVertexAllocator()   { return meshlet_vertices.get(); }
			LinearAllocator<Uint32>* GetMeshletTriangleAllocator() { return meshlet_triangles.get(); }

		private:
			Resource* Cache(std::unique_ptr<Resource>& resource, ResourceLoadDesc& load_desc) {
				ZoneScopedN("ResourceCache::Cache");

				if (!load_desc.uuid) {
					load_desc.uuid = CESAR_GEN_UUID;
				}

				resource->name = load_desc.file_path.stem().string();
				resource->uuid = load_desc.uuid;
				resources[static_cast<Uint32>(load_desc.type)][load_desc.uuid].swap(resource);
				path_uuid_map[load_desc.file_path] = load_desc.uuid;
				return resources[static_cast<Uint32>(load_desc.type)][load_desc.uuid].get();
			}

			IResourceIO* GetResourceIO(ResourceType type);

			Bool InitializeAllocators();
			void LoadDefaultResources();

		protected:
			friend class MeshIO;
			friend class ImageTextureIO;
			friend class MaterialIO;

			OfflineContext* offline_context;
			std::unique_ptr<LinearAllocator<MaterialData>> material_data;

			std::unique_ptr<LinearAllocator<Vertex>> vertices;
			std::unique_ptr<LinearAllocator<Uint32>> indices;
			std::unique_ptr<LinearAllocator<SubMeshData>> submesh_data;

			std::unique_ptr<LinearAllocator<Meshlet>> meshlet_data;
			std::unique_ptr<LinearAllocator<Uint32>> meshlet_vertices;
			std::unique_ptr<LinearAllocator<Uint32>>  meshlet_triangles;
		private:
			std::unordered_map<std::filesystem::path, UUID> path_uuid_map;
			std::array<std::unordered_map<UUID, std::unique_ptr<Resource>>, static_cast<Uint32>(ResourceType::Unknown)> resources;

			std::array<std::unique_ptr<IResourceIO>, static_cast<Uint32>(ResourceType::Unknown)> resource_io;
	};
}


/*
   === FEATURES ===

   .Load Resource Linked

  === === === === ===
*/