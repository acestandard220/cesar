#pragma once
#include "../../cesar_core/cesar_core.h"
#include "Entity.h"
#include "../../cesar_asset/Resources/Mesh.h"

#include <string>

namespace cesar {

	constexpr Uint32 START_LIGHT_COUNT = 1000;

	struct TagComponent {
		std::string tag;

		TagComponent()
			:tag("Unnamed Component")
		{

		}
		TagComponent(const std::string& name)
			:tag(name)
		{

		}
	};

	struct RelationshipComponent {
		entt::entity parent;
		std::vector<entt::entity> children;

		RelationshipComponent()
			:parent(entt::null)
		{

		}
	};

	struct LightComponent
	{
		Vector4 position;
		Vector4 direction;
		Vector4 color;
	};

	struct SubmeshInstance 
	{
		Matrix model_matrix;
		Uint32 material_index;
	};
	
	struct GPU_STRUCTURE MeshInstanceComponent {
		Matrix model_matrix;
		Uint32 submesh_data_start     = 0;
		Uint32 submesh_instance_start = 0;
		Uint32 submesh_data_count     = 0;
	};

	struct GPU_STRUCTURE MeshInstanceBatch
	{
		Uint32 instance_start = 0;
		Uint32 instance_count = 0;
	};

}