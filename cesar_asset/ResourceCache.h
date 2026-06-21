#pragma once
#include "../cesar_core/cesar_core.h"
#include "../cesar_core/Core/Filespace.h"
#include "../cesar_core/Allocator/LinearAllocator.h"

#include "../cesar_render_core/OfflineContext.h"

#include "Resource.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Resources/ImageTexture.h"

#include "ResourceIO/MeshIO/MeshIO.h"
#include "ResourceIO/MaterialIO.h"
#include "ResourceIO/ResourceIO.h"
#include "ResourceIO/ImageTextureIO.h"
#include "TextureLoadManager.h"

#include <Tracy/tracy/Tracy.hpp>

namespace cesar
{

	class __declspec(dllexport) ResourceCache 
	{
		public:
			ResourceCache(OfflineContext* offline_context, const filespace::filepath& asset_path);
			~ResourceCache() = default;

			UUID IsLoaded(const filespace::filepath& path)const {
				auto it = path_uuid_map.find(path);
				return (it != path_uuid_map.end()) ? it->second : CESAR_INVALID_UUID;
			}

			filespace::filepath GetCookedAssetPath(const filespace::filepath& raw_path)const {
				ZoneScopedN("ResourceCache::GetCookedAssetPath")

				filespace::filepath new_path = raw_path;
				if (filespace::IsChild(assets_path, raw_path))
				{
					new_path.replace_extension(extensions::cooked_asset);
					return new_path;
				}
				
				new_path = assets_path / raw_path.stem();
				new_path.replace_extension(extensions::cooked_asset);

				return new_path;
			}

			Bool IsCached(filespace::filepath& path)const {
				filespace::filepath cooked_path = GetCookedAssetPath(path);
				if (filespace::Exists(cooked_path)) {
					path = cooked_path;
					return true;
				}

				return false;
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
				ZoneScopedN("ResourceCache::LoadResource")

				if (!std::filesystem::exists(load_desc.file_path) && !load_desc.no_path)
				{
					LOG_ERROR("SPECIFIED RESOURCE PATH DOES NOT EXIST");
					return nullptr;
				}

				if (IsCached(load_desc.file_path))
					load_desc.is_cooked = true;
				else
					load_desc.is_cooked = false;

				UUID uuid = IsLoaded(load_desc.file_path);

				if (uuid) {
					LOG_WARN("RESOURCE HAS ALREADY BEEN LOADED");
					return static_cast<T*>(resources[static_cast<Uint32>(load_desc.type)][uuid].get());
				}


				IResourceIO* io = GetResourceIO(load_desc.type);
				std::unique_ptr<Resource> resource = io->LoadFromFile(load_desc);
				if (!resource) {
					LOG_ERROR("FAILED TO LOAD RESOURCE");
					return nullptr;
				}

				auto* ret = static_cast<T*>(Register(resource, load_desc));
				LOG_INFO("RESOURCE LOADED SUCCESSFULLY. ({})", ret->GetResourceName().c_str());
				return ret;
			}

			template<IsResourceType T>
			void SaveResource(Resource* resource)
			{
				filespace::filepath cooked_path = resource->GetCookedPath();
				if (IsCached(cooked_path))
					return;

				ResourceSaveDesc save_desc{};
				save_desc.save_path = cooked_path;
				save_desc.type = ResourceTypeTrait<T>::type;
				save_desc.uuid = resource->GetUUID();

				IResourceIO* io = GetResourceIO(save_desc.type);
				io->SaveToDisk(save_desc, resource);
			}

			const filespace::filepath& GetAssetsPath()const;

			ImageTexture* GetDefaultTexture(MapType type);

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
			Resource* Register(std::unique_ptr<Resource>& resource, ResourceLoadDesc& load_desc) {
				ZoneScopedN("ResourceCache::Cache");

				if (!load_desc.uuid) {
					load_desc.uuid = CESAR_GEN_UUID;
				}

				resource->cooked_path = load_desc.file_path;

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

			filespace::filepath assets_path;

			OfflineContext* offline_context;
			std::unique_ptr<LinearAllocator<MaterialData>> material_data;

			std::unique_ptr<LinearAllocator<Vertex>> vertices;
			std::unique_ptr<LinearAllocator<Uint32>> indices;
			std::unique_ptr<LinearAllocator<SubMeshData>> submesh_data;

			std::unique_ptr<LinearAllocator<Meshlet>> meshlet_data;
			std::unique_ptr<LinearAllocator<Uint32>> meshlet_vertices;
			std::unique_ptr<LinearAllocator<Uint32>>  meshlet_triangles;

			std::unique_ptr<TextureJobManager> texture_loader;
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