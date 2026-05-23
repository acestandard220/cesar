#include "Entity.h"
#include "Component.h"

namespace cesar {

	std::string_view Entity::GetTag() {
		CESAR_ASSERT(HasComponent<TagComponent>() && "Entity must has a tag component");
		return registery->get<TagComponent>(entity).tag;
	}

	void Entity::AddChildEntity(Entity& entt) {
		CESAR_ASSERT(HasComponent<RelationshipComponent>() && "Entity must have a relationship component");
		RelationshipComponent& relationship_component = registery->get<RelationshipComponent>(entity);
		relationship_component.children.push_back(entt);

		relationship_component = entt.GetComponent<RelationshipComponent>();
		relationship_component.parent = entity;
	}

	const std::vector<entt::entity>& Entity::GetChildren()
	{
		CESAR_ASSERT(HasComponent<RelationshipComponent>() && "Entity must have a relationship component");
		RelationshipComponent& relationship_component = registery->get<RelationshipComponent>(entity);
		return relationship_component.children;
	}

	const entt::entity& Entity::GetParent()
	{
		CESAR_ASSERT(HasComponent<RelationshipComponent>() && "Entity must have a relationship component");
		RelationshipComponent& relationship_component = registery->get<RelationshipComponent>(entity);
		return relationship_component.parent;
	}

}