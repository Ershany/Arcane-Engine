#pragma once

#include <Arcane/Scene/Components.h>
#include <Arcane/Scene/Scene.h>

#include "entt.hpp"

namespace Arcane
{
	// Lightweight class that represents and holds the functionality of an entity
	// Shouldn't pass by reference, it should stay lightweight and be constructed and copied when needed
	class Entity
	{
	public:
		Entity() = default;
		Entity(Scene *scene, entt::entity handle) : m_Scene(scene), m_Handle(handle) {}

		TransformComponent& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_Handle); }
		const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_Handle).GetTransform(); }

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			ARC_ASSERT(!HasComponent<T>(), "Entity already has this component");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		void RemoveComponent()
		{
			ARC_ASSERT(HasComponent<T>(), "Entity does not have this component");
			return m_Scene->m_Registry.remove<T>(m_Handle);
		}

		template<typename T>
		T& GetComponent()
		{
			ARC_ASSERT(HasComponent<T>(), "Entity does not have this component");
			m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.any_of<T>(m_Handle);
		}

		template<typename... T>
		bool HasAnyComponents() const
		{
			return m_Scene->m_Registry.any_of<T...>(m_Handle);
		}

		template<typename... T>
		bool HasAllComponents() const
		{
			return m_Scene->m_Registry.all_of<T...>(m_Handle);
		}

		operator bool() const { return (m_Handle != entt::null) && m_Scene; }

		bool operator==(const Entity &other) const
		{
			return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity &other) const
		{
			return !(*this == other);
		}
	private:
		entt::entity m_Handle{ entt::null };
		Scene *m_Scene = nullptr;
	};
}