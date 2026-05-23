#pragma once
#include "../../cesar_core/cesar_core.h"


#include <type_traits>

#include <entt/entt.hpp>

namespace cesar {

	class Entity {
	public:
		Entity()
			:registery(nullptr), entity(entt::null)
		{

		}

		Entity(entt::registry* reg, entt::entity entity)
			:registery(reg), entity(entity)
		{
			entt::registry rr;
		}
		~Entity() {
			
		}

		Bool IsValid()const { return entity == entt::null; }

		template<typename T>
		Bool HasComponent() {
			return registery->any_of<T>(entity);
		}

		template<typename T>
		T& GetComponent() {
			CESAR_ASSERT(HasComponent<T>() && "Cannot access a component from an entity which does not have it");
			return registery->get<T>(entity);
		}

		std::string_view GetTag();

		template<typename T, typename... Args>
			requires std::is_constructible<T>::value
		T& AddComponent(Args&& ...args) {
			return registery->emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent() {
			CESAR_ASSERT(HasComponent<T>() && "Cannot remove component from an entity that does not have it.");
			registery->remove<T>(entity);
		}


		void AddChildEntity(Entity& entt);
		const std::vector<entt::entity>& GetChildren();
		const entt::entity& GetParent();

		operator entt::entity()const {
			return entity;
		}
	private:
		entt::registry* registery;
		entt::entity entity = entt::null;
	};

}