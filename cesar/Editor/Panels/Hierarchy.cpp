#include "Hierarchy.h"
#include "../Editor.h"

#include "../../../cesar_core/Event/Input.h"
#include "../../../cesar_scene/Entity/Entity.h"
#include <ImGui/imgui.h>

namespace cesar
{
	Hierarchy::Hierarchy(Editor* editor)
		:editor(editor), active(true),name("Hierarchy")
	{
	
	}

	Hierarchy::~Hierarchy()
	{
	
	}

	void Hierarchy::DrawHierarchy(Scene* scene)
	{
		ImGui::Begin(name, &active);

		auto& entities = scene->GetEntities();
		auto reg = &scene->GetEnttRegistery();

		std::function<void(Entity& entt)> ShowEntity;
		ShowEntity = [&](Entity& entt) {
			const auto& tag = entt.GetComponent<TagComponent>().tag;
		    auto& rel = entt.GetComponent<RelationshipComponent>();

			int flags = ImGuiTreeNodeFlags_OpenOnDoubleClick;

			if (rel.children.empty())
			{
				flags |= ImGuiTreeNodeFlags_Leaf;
			}

			if (selected_entity == entt) {
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			if (ImGui::TreeNodeEx(tag.c_str(), flags))
			{
				if (ImGui::IsItemClicked()) {
					selected_entity = entt;
				}

				for (entt::entity& child_entt : rel.children)
				{
					Entity child{ reg, child_entt };
					ShowEntity(child);
				}
				ImGui::TreePop();
			}
	    };

		for (auto& entt : entities)
		{
			const auto& rel = entt.GetComponent<RelationshipComponent>();

			if (rel.parent == entt::null)
			{
				ShowEntity(entt);
			}
		}

		ImGui::End();
	}
}
