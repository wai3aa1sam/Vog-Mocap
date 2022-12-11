#pragma once
#include "Vog/Core/Core.h"

//#include "Components.h"
#include "Scene.h"

namespace vog {

	class VOG_API Entity
	{
		friend class Scene;
	public:
		Entity() = default;
		Entity(ecs::EntityHandle handle_, Scene* pScene_);
		Entity(ecs::EntityHandle handle_, Entity other_);
		//Entity(ecs::EntityHandle handle_);

		~Entity() = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args_)
		{
			VOG_CORE_ASSERT(m_pScene != nullptr, "");
			VOG_CORE_ASSERT(!hasComponent<T>(), "");

			T& component = m_pScene->m_ecs.addEntityComponent<T>(m_handle, std::forward<Args>(args_)...);
			m_pScene->_onComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		void removeComponent()
		{
			VOG_CORE_ASSERT(m_pScene != nullptr, "");
			VOG_CORE_ASSERT(hasComponent<T>(), "");
			m_pScene->_onComponentRemoved<T>(*this);
			m_pScene->m_ecs.removeEntityComponent<T>(m_handle);
		}

		// adding component to its Manager maybe lead to resize problem!
		template<typename T>
		T& getComponent()					
		{
			VOG_CORE_ASSERT(m_pScene != nullptr, "");
			VOG_CORE_ASSERT(hasComponent<T>(), "");
			return m_pScene->m_ecs.getComponent_unsafe<T>(m_handle);
		}

		// adding component to its Manager maybe lead to resize problem!
		template<typename T>
		const T& getComponent() const		
		{
			VOG_CORE_ASSERT(m_pScene != nullptr, "");
			VOG_CORE_ASSERT(hasComponent<T>(), "");
			return m_pScene->m_ecs.getComponent_unsafe<T>(m_handle);
		}

		template<typename T> bool hasComponent() { return m_pScene->m_ecs.isEntityHasComponent<T>(m_handle); }
		template<typename T> bool hasComponent() const { return m_pScene->m_ecs.isEntityHasComponent<T>(m_handle); }

		operator uint32_t() { return m_handle; }

		Scene* getpScene() { return m_pScene; }

		//const std::string& getName() { return getComponent<TagComponent>().tag; }

	private:
		ecs::EntityHandle m_handle = ecs::s_invalid_entity;
		Scene* m_pScene = nullptr;
	};
}