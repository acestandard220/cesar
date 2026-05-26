#include "Scene.h"
#include "Entity/Component.h"
#include "../cesar_core/Core/MathConstants.h"

using namespace cesar;
using namespace cesar;

namespace cesar
{
	Scene::Scene(Window* window, ResourceCache* resource_cache)
		:resource_cache(resource_cache)
	{
		Uint32 client_width, client_height;
		window->GetClientDimensions(client_width, client_height);

		CameraParameters camera_parameters;
		camera_parameters.position = { 0.0,2.0,-8.0 };
		camera_parameters.look_at = { 0.0,0.0,1.0 };
		camera_parameters.fov = 45.0f;
		camera_parameters.near_plane = 0.1f;
		camera_parameters.far_plane = 1600.0f;
		editor_camera = std::make_unique<Camera>(camera_parameters);

		scene_cameras.emplace_back(camera_parameters);

		submesh_instances = std::make_unique<LinearAllocator<SubmeshInstance>>();
		if (!submesh_instances->Reserve(1000)) {
			LOG_FATAL("Failed to initialize submesh data pool.");
			return;
		}
	}

	Scene::~Scene()
	{
        
	}

	void Scene::OnWindowEvent(const WindowEventInfo& event_info)
	{
		
	}
	
	void Scene::OnResize(Uint32 w, Uint32 h)
	{
		this->editor_camera->SetAspectRatio((float)w / (float)h);

		for (auto& cam : scene_cameras) {
			cam.OnResize(w, h);
		}
	}

	Entity Scene::CreateEmptyEntity(const std::string& name)
	{
		Entity entity(&registery, registery.create());
		entity.AddComponent<TagComponent>(name);
		entity.AddComponent<RelationshipComponent>();

		entities.push_back(entity);
		return entity;
	}

	Entity Scene::AddMeshEntity(const std::filesystem::path& path)
	{
		MeshLoadDesc load_desc{};
		load_desc.file_path = path;
		load_desc.type = ResourceType::Mesh;
		
		Mesh* mesh_resource = resource_cache->LoadResource<Mesh>(load_desc);
		SubMeshData* mesh_data = resource_cache->GetSubMeshDataBlock(mesh_resource->submesh_start,mesh_resource->submesh_data_count).data();
		total_instance_submesh += mesh_resource->submesh_data_count;
		total_meshlet += mesh_resource->meshlet_count;

		if (mesh_resource == nullptr) {
			LOG_ERROR("Failed to load mesh resource. [Path]::[{}]", path.string().c_str());
			return Entity();
		}

		const UUID& uuid = mesh_resource->GetUUID();
		const Uint32 existing_instance_count = GetMeshInstanceCount(uuid);

		const std::string name = std::format("{}{}", mesh_resource->GetResourceName(),
			existing_instance_count ? std::format("_{}", existing_instance_count) : "");
		
		Entity entity = CreateEmptyEntity(name);
		//RelationshipComponent& relation_component = 
		MemoryBlock<SubmeshInstance> smd_block = submesh_instances->Allocate(mesh_resource->submesh_data_count);
		Uint32 submesh_index = 0;
		for (auto& sm_instance : smd_block) {
			Entity _submesh_entity = CreateEmptyEntity(mesh_resource->submesh_names[submesh_index]);
			entity.AddChildEntity(_submesh_entity);

			//TODO: Set default materials
			//sm_instance.material_index = mesh_resource->default_materials[submesh_index];
			sm_instance.material_index = 0;
			sm_instance.model_matrix = mesh_resource->model_matrixes[submesh_index];
			submesh_index++;
		}

		Uint32 smd_start = submesh_instances->GetIndex(smd_block);

		MeshInstanceComponent& mesh_filter   = entity.AddComponent<MeshInstanceComponent>();
		mesh_filter.model_matrix             = mesh_resource->model_matrix;
		mesh_filter.submesh_data_start       = mesh_resource->submesh_start;
		mesh_filter.submesh_instance_start   = smd_start;
		mesh_filter.submesh_data_count       = mesh_resource->submesh_data_count;

		InsertMeshInstance(uuid, entity);

		mesh_resource->Use();
		return entity;
	}

	Entity Scene::AddLightEntity(std::string const& name)
	{
		Entity entity = CreateEmptyEntity(name);
		LightComponent light = entity.AddComponent<LightComponent>();
		light.position = { 0.0f, 0.0f, 0.0f, 1.0f };
		light.direction = { 0.0f, 0.0f, 0.0f, 0.0f };
		light.color = { 1.0f, 1.0f, 1.0f, 1.0f };

		total_light++;
		return entity;
	}

	cesar::ResourceCache* Scene::GetResourceCache() const
	{
		return resource_cache;
	}

	std::vector<Entity>& Scene::GetMeshInsances(UUID uuid) 
	{
		return mesh_instances[uuid];
	}

	const Uint64& Scene::GetTotalInstanceSubmesh()
	{
		return total_instance_submesh;
	}

	const Uint64& Scene::GetTotalMeshlet() const
	{
		return total_meshlet;
	}

	const Uint64& Scene::GetTotalLight() const
	{
		return total_light;
	}

	Uint32 Scene::GetMeshInstanceCount(UUID uuid)
	{
		if (mesh_instances.find(uuid) == mesh_instances.end()) {
			return 0;
		}
		return mesh_instances[uuid].size();
	}
	const Uint64& Scene::GetTotalMeshInstance() const
	{
		return total_mesh_instance;
	}
	void Scene::InsertMeshInstance(UUID id, Entity& entt)
	{
		mesh_instances[id].push_back(entt);
		total_mesh_instance++;
	}
}