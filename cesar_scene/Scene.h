#pragma once
#include "../cesar_core/Platform/Window.h"
#include "../cesar_asset/ResourceCache.h"

#include "Camera.h"

#include "Entity/Entity.h"
#include "Entity/Component.h"

#include <entt/entt.hpp>

namespace cesar
{
	class _declspec(dllexport) Scene
	{
	public:
		Scene(cesar::Window* window, ResourceCache* resource_cache);
		~Scene();
		
		void OnWindowEvent(const WindowEventInfo& event_info);
		void OnResize(Uint32 w, Uint32 h);

		Entity CreateEmptyEntity(const std::string& name = "Empty Component");
		
		Entity AddMeshEntity(UUID uuid);
		Entity AddMeshEntity(const std::filesystem::path& path);

		Camera* GetEditorCamera()const { return editor_camera.get(); }
		Camera* GetActiveSceneCamera() { return &scene_cameras.back(); }

		ResourceCache* GetResourceCache()const;

		template<typename T>
		std::vector<entt::entity> GetEntitiesWith() {
			return registery.view<T>();
		}

		const Uint64& GetTotalMeshInstance()const;
		const Uint64& GetTotalInstanceSubmesh();
		const Uint64& GetTotalMeshlet()const;

		Uint32 GetMeshInstanceCount(UUID uuid);
		std::vector<Entity>& GetMeshInsances(UUID uuid);

		LinearAllocator<SubmeshInstance>* GetSubmeshInstanceAllocator() { return submesh_instances.get(); }

		entt::registry& GetEnttRegistery() { return registery; }
	private:
		void InsertMeshInstance(UUID id, Entity& entt);
	private:
		entt::registry registery;
		std::vector<Entity> entities;

		std::unique_ptr<Camera> editor_camera = nullptr; //todo: Move to editor
		std::vector<Camera> scene_cameras;

		ResourceCache* resource_cache;

		std::unordered_map<UUID, std::vector<Entity>> mesh_instances;

		Uint64 total_mesh_instance = 0;
		Uint64 total_instance_submesh = 0;
		Uint64 total_meshlet = 0;

		std::unique_ptr<LinearAllocator<SubmeshInstance>> submesh_instances = nullptr;
	};

}