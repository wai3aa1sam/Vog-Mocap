#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Objects/Camera.h"

#include "ECS.h"

/*
*	Important: if appending ecs stuff, espcailly that Component has vector(dynamic array) (), (eg. referencing the HierarchyComponent in duplicate or create Entity in scene)
*	the vector in that ComponentManager maybe resized, getting the pointer of that maybe invaild after operations (especially recursion)
*/

namespace vog {

	class Entity;
	class Model;
	struct AssimpNodeData;

	class PhysicsWorld;
	namespace Test { class PhysicsEngine; }		// TODO: remove

	class VOG_API Scene : public NonCopyable
	{
		friend class ScenePanel;

		friend struct PhysicsSystem;
	public:
		Scene();
		~Scene();

		static RefPtr<Scene> copy(const RefPtr<Scene>& rhs_);

		Entity createEntity(const std::string& name_);
		Entity createEntity(const std::string& name_, Entity parent_);

		void onRuntimeStart();
		void onRuntimeStop();

		void onUpdateRuntime(float timestep_, Camera& camera_);
		void onUpdateEditor(float timestep_, Camera& camera_);

		void raycast(const Vector3f origin_, const Vector3f direction_);

		Entity loadModel(const filepath& filepath_, Entity parent_);
		Entity loadModel(const filepath& filepath_);

		inline ecs::ECS& getECS() { return m_ecs; }

		void destroyEntity(Entity entity_);
		void destroyEntity_sorted(Entity entity_);

		Entity duplicateEntity(Entity entity_, Entity parent_);

		template<typename T>
		ecs::ComponentManager<T>& getComponentManager();

		template<typename T>
		size_t getComponentCount();

	private:
		void _loadModelHierarchy(Entity dstEntity_, Entity parentEntity_, const AssimpNodeData& parentNode_, const Matrix4f& parentMatrix_ /*useless now*/, const RefPtr<Model>& pModel_, int level_);
		void _duplicateEntityHierarchy(Entity parent_, Entity src_, int level_);

		template<typename T>
		void _onComponentAdded(Entity entity_, T& component_);

		template<typename T>
		void _onComponentRemoved(Entity entity_);

		//template<typename T> void onComponentAdded_Impl(Entity entity_, T& component_);

	private:
		ecs::ECS m_ecs;

		std::unique_ptr<Test::PhysicsEngine> m_pPhysicsEngine;

		bool m_isRuntime = false;

		PhysicsWorld* m_pPhysicsWorld = nullptr;

		friend class Entity;
	};
	
	template<typename T>
	inline ecs::ComponentManager<T>& Scene::getComponentManager() { return m_ecs.getComponentManager<T>(); }	// Warn: will change due to resize! if the cm vector is appended

	template<typename T>
	inline size_t Scene::getComponentCount() { return m_ecs.getComponentManager<T>().getCount(); }


	//template<typename T>
	//inline void Scene::onComponentAdded(Entity entity_, T& component_)
	//{
	//	if (ecs::ECS::getComponentTypeID<T>() == ecs::ECS::getComponentTypeID<RigidbodyComponent>())
	//	{
	//		//RigidbodyComponent& rigidbody = component_;
	//		this->onComponentAdded_Impl(entity_, reinterpret_cast<T&>(component_));
	//		VOG_CORE_LOG_INFO("RigidbodyComponent onAdded!");

	//	}
	//	else
	//		VOG_CORE_LOG_INFO("T onAdded!");
	//}
}