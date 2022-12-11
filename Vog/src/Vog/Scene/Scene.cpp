#include "vogpch.h"
#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Systems.h"

#include "Vog/Graphics/Renderer/Renderer.h"
#include "Vog/Graphics/Renderer/DeferredRenderer.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/Graphics/Mesh.h"

#include "Vog/Physics/Physics.h"

#include "Vog/Scene/NativeScriptEntity.h"

#include "Vog/Objects/Light.h"

namespace vog {

//#define node_base_load_model
#define ENABLE_native_script

	namespace Utils {
		template<typename Component>
		void copyComponentIfExit(Entity dst_, Entity src_)
		{
			if (src_.hasComponent<Component>())
				dst_.addComponent<Component>(src_.getComponent<Component>());
		}
	}

	Scene::Scene()
	{
		m_ecs.registerComponent<LightComponent>()->reserve(Light::s_max_light_count);
		m_ecs.registerComponent<LightComponent::Type>()->reserve(Light::s_max_light_count);
		m_ecs.registerComponent<LightComponent::Position>()->reserve(Light::s_max_light_count);
		m_ecs.registerComponent<LightComponent::Direction>()->reserve(Light::s_max_light_count);
		m_ecs.registerComponent<LightComponent::Color>()->reserve(Light::s_max_light_count);
		m_ecs.registerComponent<LightComponent::Params>()->reserve(Light::s_max_light_count);

		// TODO: remove, for demo game loading speed
		m_ecs.registerComponent<NativeScriptComponent>()->reserve(1000);
		m_ecs.registerComponent<RigidbodyComponent>()->reserve(1000);
		m_ecs.registerComponent<OBBColliderComponent>()->reserve(1000);
	}

	Scene::~Scene()
	{
		m_pPhysicsEngine.reset(nullptr);
		delete m_pPhysicsWorld;
		m_pPhysicsWorld = nullptr;
	}

	RefPtr<Scene> Scene::copy(const RefPtr<Scene>& rhs_)
	{
		auto pNewScene = createRefPtr<Scene>();
		pNewScene->m_ecs = rhs_->m_ecs;
		return pNewScene;
	}

	Entity Scene::createEntity(const std::string& name_)
	{
		Entity entity = Entity{ m_ecs.createEntity(), this };
		m_ecs.attach(entity, {});

		entity.addComponent<IDComponent>().id = UUID();
		auto& tag = entity.addComponent<TagComponent>();
		entity.addComponent<TransformComponent>();
		tag.tag = name_.empty() ? "Entity" : name_;
		return entity;
	}

	Entity Scene::createEntity(const std::string& name_, Entity parent_)
	{
		Entity entity = Entity{ m_ecs.createEntity(), this };
		m_ecs.attach(entity, parent_);

		entity.addComponent<IDComponent>().id = UUID();
		auto& tag = entity.addComponent<TagComponent>();
		entity.addComponent<TransformComponent>();
		tag.tag = name_.empty() ? "Entity" : name_;
		return entity;
	}

	void Scene::onRuntimeStart()
	{

		m_pPhysicsEngine = std::make_unique<Test::PhysicsEngine>(Vector3f(0.0f, -9.81f, 0.0f));

		// TODO: remove test
		auto& particleSystem_cm = m_ecs.getComponentManager<ParticleSystemComponent>();
		auto& particleSystemComponents = particleSystem_cm.getComponents();

		for (size_t i = 0; i < particleSystem_cm.getCount(); i++)
		{
			particleSystemComponents[i].pParticles = m_pPhysicsEngine->m_particleSystem.m_pParticlesBuffer;
			m_pPhysicsEngine->m_particleSystem.m_currentParticlesCount = particleSystemComponents[i].particlesCount;
		}

		//----------------

		NativeScriptSystem::onAwake(this);

		PhysicsSystem::onStart(this);			// new m_pPhysicsWorld here;

		m_isRuntime = true;

#ifdef ENABLE_native_script
		NativeScriptSystem::onStart(this);
#endif // ENABLE_native_script
	}

	void Scene::onRuntimeStop()
	{
#ifdef ENABLE_native_script
		NativeScriptSystem::onDestroy(this);
#endif // ENABLE_native_script

		// TODO: remove test
		auto& particleSystem_cm = m_ecs.getComponentManager<ParticleSystemComponent>();
		auto& particleSystemComponents = particleSystem_cm.getComponents();

		for (size_t i = 0; i < particleSystem_cm.getCount(); i++)
		{
			particleSystemComponents[i].pParticles = m_pPhysicsEngine->m_particleSystem.m_pParticlesBuffer;
			particleSystemComponents[i].pParticleVertexBuffer->setData(nullptr, 0);
		}

		// TODO: remove test
		m_pPhysicsEngine.reset(nullptr);

		delete m_pPhysicsWorld;
		m_pPhysicsWorld = nullptr;

		m_isRuntime = false;
	}

	void Scene::onUpdateRuntime(float timestep_, Camera& camera_)
	{
		//Renderer::beginScene()

		auto& ecs = getECS();
		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& hierarchy_cm = ecs.getComponentManager<ecs::HierarchyComponent>();
		VOG_CORE_ASSERT(transform_cm.getCount() == hierarchy_cm.getCount(), "");

		TransformHierarchySystem::update(this);

		BoneTransformSystem::update(this);
		AnimationSystem::update(timestep_, this);

		// Physics
		{
			PhysicsSystem::uploadData(this);

			PhysicsSystem::update(m_pPhysicsWorld, timestep_, this);
			//TestPhysicsSystem::update(m_pPhysicsEngine.get(), timestep_, this);
		}

		// script
		{
#ifdef ENABLE_native_script
			NativeScriptSystem::update(this, timestep_);
#endif // ENABLE_native_script

		}

		// Render
		{
			MeshRendererSystem::uploadLights(this);

			DeferredRenderer::beginScene(camera_);

			MeshRendererSystem::update(this);
			//TestRenderSystem::update(this);

			DeferredRenderer::endScene();
		}
	}

	void Scene::onUpdateEditor(float timestep_, Camera& camera_)
	{
		auto& ecs = getECS();
		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& hierarchy_cm = ecs.getComponentManager<ecs::HierarchyComponent>();
		VOG_CORE_ASSERT(transform_cm.getCount() == hierarchy_cm.getCount(), "");

		TransformHierarchySystem::update(this);

		BoneTransformSystem::update(this);
		AnimationSystem::update(timestep_, this);

		// Render
		{
			MeshRendererSystem::uploadLights(this);

			DeferredRenderer::beginScene(camera_);

			MeshRendererSystem::update(this);
			//TestRenderSystem::update(this);

			DeferredRenderer::endScene();
		}
	}

	void Scene::raycast(const Vector3f origin_, const Vector3f direction_)
	{
		if (m_pPhysicsWorld)
		{
			m_pPhysicsWorld->raycast(origin_, direction_);
		}
	}

	void Scene::destroyEntity(Entity entity_)
	{
		if (this->m_isRuntime && entity_.hasComponent<RigidbodyComponent>())
		{
			auto& rigidbody = entity_.getComponent<RigidbodyComponent>();
			m_pPhysicsWorld->removeBody(static_cast<Body*>(rigidbody.pRuntimeBody)->getHandle());
		}
		if (entity_.hasComponent<LightComponent>())
			entity_.removeComponent<LightComponent>();		// trigger on remove

		m_ecs.destroyEntity(entity_);
		entity_.m_handle = ecs::s_invalid_entity;
	}

	void Scene::destroyEntity_sorted(Entity entity_)
	{
		if (this->m_isRuntime && entity_.hasComponent<RigidbodyComponent>())
		{
			auto& rigidbody = entity_.getComponent<RigidbodyComponent>();
			m_pPhysicsWorld->removeBody(static_cast<Body*>(rigidbody.pRuntimeBody)->getHandle());
		}

		if (entity_.hasComponent<LightComponent>())
			entity_.removeComponent<LightComponent>();		// trigger on remove


		m_ecs.destroyEntity_sorted(entity_);
		entity_.m_handle = ecs::s_invalid_entity;
	}

	static void copyAllComponentIfExit(Entity dst_, Entity src_)
	{
			Utils::copyComponentIfExit<MaterialComponent			 >(dst_, src_);
			Utils::copyComponentIfExit<MeshComponent				 >(dst_, src_);
			Utils::copyComponentIfExit<MeshRendererComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<AnimationComponent			 >(dst_, src_);
			Utils::copyComponentIfExit<BoneTransformComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<SpriteRendererComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<NativeScriptComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<BoxColliderComponent			 >(dst_, src_);
			Utils::copyComponentIfExit<CapsuleColliderComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<SphereColliderComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<PlaneColliderComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<AABBColliderComponent		 >(dst_, src_);
			Utils::copyComponentIfExit<OBBColliderComponent			 >(dst_, src_);
			Utils::copyComponentIfExit<TriangleColliderComponent	 >(dst_, src_);

			Utils::copyComponentIfExit<RigidbodyComponent>			(dst_, src_);
			{
				Utils::copyComponentIfExit<LightComponent>(dst_, src_);
				Utils::copyComponentIfExit<LightComponent::Type>(dst_, src_);
				Utils::copyComponentIfExit<LightComponent::Position>(dst_, src_);
				Utils::copyComponentIfExit<LightComponent::Direction>(dst_, src_);
				Utils::copyComponentIfExit<LightComponent::Color>(dst_, src_);
				Utils::copyComponentIfExit<LightComponent::Params>(dst_, src_);
			}
	}

	Entity Scene::duplicateEntity(Entity entity_, Entity parent_)
	{
		const auto& hierarchy = entity_.getComponent<ecs::HierarchyComponent>();

		std::string name = entity_.getComponent<TagComponent>().tag;

		auto parentID = parent_ ? parent_ : hierarchy.parentID;
		auto entity = createEntity(name, { parentID, this});

		copyAllComponentIfExit(entity, entity_);

		//VOG_CORE_LOG_INFO("{0} level: {1}, name: {2}, parentID: {3}, no. of children: {4}", getIndentationLevel(0), 0, name, hierarchy.parentID, hierarchy.children.size());

		auto temp = hierarchy.children;		// must copy / reserve the hierarchy components with a large capacity!
		for (uint32_t i = 0; i < temp.size(); ++i)
		{
			_duplicateEntityHierarchy(entity, Entity{ temp[i], this }, 1);

			// recusive function maybe trigging resize, the component maybe invaild now!
			// create Entity will auto attach the children, dont push back!!!
		}

		return entity;
	}

	Entity Scene::loadModel(const filepath& filepath_, Entity parent_)
	{
		RefPtr<Model>& pModel = AssetManager::loadMesh(filepath_.string());

		Model& model = *pModel;

		//auto pShader = (model.isAnimation() ? AssetManager::getShader("Animation") : AssetManager::getShader("Simple_Model_Loading"));
		auto pShader = (model.isAnimation() ? AssetManager::getRendererAnimatedShader() : AssetManager::getRendererShader());

		model.loadMaterial(pShader);

		auto& subMeshes = model.m_subMeshes;
		auto& rootNode = model.m_rootNode;

		auto rootEntity = createEntity(model.m_name, parent_);

		_loadModelHierarchy(rootEntity, rootEntity, rootNode, Matrix4f(1.0f), pModel, 0);

		if (model.m_isAnimation)
		{
			auto& animationComponent = rootEntity.addComponent<AnimationComponent>();
			animationComponent.pModel = pModel;
		}

		return rootEntity;
	}

	Entity Scene::loadModel(const filepath& filepath_)
	{
		return loadModel(filepath_, {});
	}

	void Scene::_loadModelHierarchy(Entity dstEntity_, Entity parentEntity_, const AssimpNodeData& parentNode_,
		const Matrix4f& parentMatrix_ /*useless now*/, const RefPtr<Model>& pModel_, int level_)
	{
		// pass the cm maybe faster, but it destroy the meaning of Entity wrapper class;

		//VOG_CORE_LOG_INFO("{0} level: {1}, node name: {2}", getIndentationLevel(level_), level_, tagComponent.tag);

		auto& model = *pModel_;

		for (uint32_t i = 0; i < parentNode_.nMeshes; i++)
		{
			VOG_CORE_ASSERT(parentNode_.pMeshesIndex[i] < model.getMeshes().size(), "");
			uint32_t meshIndex = parentNode_.pMeshesIndex[i];
			auto& subMesh = model.m_subMeshes[meshIndex];

			Entity* pEntity = nullptr;
#ifndef node_base_load_model		
			// first index is current entity (parent is parent entity), following index is also base on parent entity
			// remark: the transform on UI panel is base on world (somehow)
			if (i == 0)
			{
				pEntity = &dstEntity_;
			}
			else
			{
				auto childEntity = createEntity(subMesh.name, parentEntity_);
				pEntity = &childEntity;
			}
#else		
			// all mesh will have a own entity which base on current entity (parent is parent entity)
			// remark: the transform on UI panel is base on local (somehow)
			auto childEntity = createEntity(subMesh.name, dstEntity_);
			pEntity = &childEntity;
#endif // 0

			auto& meshComponent = pEntity->addComponent<MeshComponent>();
			auto& meshRendererComponent = pEntity->addComponent<MeshRendererComponent>();

			meshComponent.pModel = pModel_;

			meshComponent.isAnimated = model.m_isAnimation;
			meshComponent.subMesh = subMesh;

			meshComponent.pVertexBuffer = model.m_pVertexBuffer;
			meshComponent.pIndexBuffer = model.m_pIndexBuffer;

			meshRendererComponent.pMaterial = model.m_materialPtrs[meshComponent.subMesh.materialIndex];

#ifndef node_base_load_model
			auto& transformComponent = pEntity->getComponent<TransformComponent>();
			MyMath::decompose(parentNode_.localTransform, transformComponent.scale, transformComponent.rotation, transformComponent.translation);
#endif // !node_base_load_model
		}

		if (pModel_->m_boneInfoMap.find(parentNode_.name) != pModel_->m_boneInfoMap.end())
		{
			auto& boneTransformComponent = dstEntity_.addComponent<BoneTransformComponent>();		// TODO: remove
			boneTransformComponent.pBoneTransform = const_cast<Transform*>(&parentNode_.transform);
		}

#ifdef node_base_load_model
		auto& transformComponent = dstEntity_.getComponent<TransformComponent>();
		MyMath::decompose(parentNode_.localTransform, transformComponent.scale, transformComponent.rotation, transformComponent.translation);
#endif // node_base_load_model

		// transformComponent.worldMatrix = parentNode_.gobalTransform;
		// or
		// transformComponent.worldMatrix = parentMatrix_ * parentNode_.localTransform;
		// parentMatrix_ = transformComponent.worldMatrix;
		
		for (uint32_t i = 0; i < parentNode_.nChildren; ++i)		// not using the hierarchy stuff avoid the problems of resizing
		{
			const auto& childNode = parentNode_.children[i];		// not using the hierarchy stuff avoid the problems of resizing
			auto childEntity = createEntity(childNode.name, dstEntity_);
			
			_loadModelHierarchy(childEntity, dstEntity_, childNode, dstEntity_.getComponent<TransformComponent>().worldMatrix, pModel_, level_ + 1);

			// recusive function maybe trigging resize, the component maybe invaild now!
			// create Entity will auto attach the children, dont push back!!!
		}
	}

	void Scene::_duplicateEntityHierarchy(Entity parent_, Entity src_, int level_)
	{
		std::string name = src_.getComponent<TagComponent>().tag;
		auto entity = createEntity(name, parent_);

		copyAllComponentIfExit(entity, src_);

		//VOG_CORE_LOG_INFO("{0} level: {1}, name: {2}, parentID: {3}, no. of children: {4}", getIndentationLevel(level_), level_, name, hierarchy.parentID, hierarchy.children.size());

		auto temp = src_.getComponent<ecs::HierarchyComponent>().children;		// must copy / reserve the hierarchy components with a large capacity!
		for (uint32_t i = 0; i < temp.size(); ++i)
		{
			_duplicateEntityHierarchy(entity, Entity{ temp[i], this }, level_ + 1);

		// create Entity will auto attach the children, dont push back!!!
		 // create Entity will auto attach the children, dont push back!!!
		}
	}

#pragma region onComponentAdded

	template<typename T>
	void Scene::_onComponentAdded(Entity entity_, T& component_)
	{
		VOG_CORE_LOG_INFO("T onAdded!");
		assert(0);
	}

	template<> void Scene::_onComponentAdded(Entity entity_, RigidbodyComponent& component_)
	{
		if (this->m_isRuntime)
		{
			VOG_CORE_LOG_INFO("RigidbodyComponent onAdded!");

			Entity entity = entity_;

			auto& rigidbody = component_;
			auto& transform = entity.getComponent<TransformComponent>();

			auto* pBody = this->m_pPhysicsWorld->addBody();
			rigidbody.pRuntimeBody = pBody;

			Vector3f translation;
			Vector3f scale;
			Vector3f rotation;
			MyMath::decompose(transform.worldMatrix, scale, rotation, translation);
			pBody->m_transform.position = translation;
			pBody->m_transform.orientation = MyMath::toQuaternion(rotation);

			pBody->m_data.velocity = rigidbody.velocity;

			pBody->m_type = (BodyType)rigidbody.type;

			if (entity.hasComponent<NativeScriptComponent>())		// should be no resize problem
			{
				auto& nativeScript = entity.getComponent<NativeScriptComponent>();
				VOG_CORE_ASSERT(nativeScript.pInstance, "");
				pBody->m_pCollisionCallbackBase = (CollisionCallbackBase*)nativeScript.pInstance;
			}

			if (entity.hasComponent<CapsuleColliderComponent>())
			{
				auto& capsuleCollider = entity.getComponent<CapsuleColliderComponent>();

				auto height = capsuleCollider.height * transform.scale.y;
				auto normal = MyMath::eulerToMatrix3f(transform.rotation) * Vector3f(0.0f, 1.0f, 0.0f);
				auto radius = MyMath::max(transform.scale.x, transform.scale.z) * capsuleCollider.radius;

				pBody->setAsCapsule(capsuleCollider.center, normal, height, radius);
			}
			else if (entity.hasComponent<SphereColliderComponent>())
			{
				auto& sphereCollider = entity.getComponent<SphereColliderComponent>();

				pBody->setAsSphere(sphereCollider.center, (sphereCollider.radius) *
					MyMath::max(transform.scale.x, MyMath::max(transform.scale.y, transform.scale.z)));
			}
			else if (entity.hasComponent<PlaneColliderComponent>())
			{
				auto& planeCollider = entity.getComponent<PlaneColliderComponent>();

				auto normal = MyMath::eulerToMatrix3f(transform.rotation) * Vector3f(0.0f, 1.0f, 0.0f);
				normal = MyMath::normalize(normal);
				float distance = MyMath::magnitude(transform.translation);
				if (transform.translation.y < 0.0f)
					distance *= -1.0f;
				PlaneShape* pPlane = new PlaneShape(normal, distance);		// TODO: rectify

				pBody->m_pShape = pPlane;
			}
			else if (entity.hasComponent<AABBColliderComponent>())		// TODO: remove temp
			{
				auto& aabbCollider = entity.getComponent<AABBColliderComponent>();

				AABBShape* pAABB = new AABBShape(aabbCollider.center, aabbCollider.size * transform.scale);

				pBody->m_pShape = pAABB;
			}
			else if (entity.hasComponent<OBBColliderComponent>())		// TODO: remove temp
			{
				auto& obbCollider = entity.getComponent<OBBColliderComponent>();

				pBody->setAsBox(obbCollider.center, obbCollider.size * transform.scale, transform.rotation);
			}
			else if (entity.hasComponent<TriangleColliderComponent>())		// TODO: remove temp
			{
				auto& triangleCollider = entity.getComponent<TriangleColliderComponent>();

				Vector4f trianglePositions[3] = {
				Vector4f(-0.5f, 0.0f,  0.5f, 1.0f),
				Vector4f(-0.5f, 0.0f, -0.5f, 1.0f),
				Vector4f(0.5f, 0.0f,  0.5f, 1.0f),
				};

				TriangleShape* pTriangle = new TriangleShape(trianglePositions[0], trianglePositions[1], trianglePositions[2]);

				pBody->m_pShape = pTriangle;
			}
		}
	}

	template<> void Scene::_onComponentAdded(Entity entity_, LightComponent& component_) 
	{
		entity_.m_pScene->m_ecs.addEntityComponent<LightComponent::Type>(entity_);
		entity_.m_pScene->m_ecs.addEntityComponent<LightComponent::Position>(entity_);
		entity_.m_pScene->m_ecs.addEntityComponent<LightComponent::Direction>(entity_);
		entity_.m_pScene->m_ecs.addEntityComponent<LightComponent::Color>(entity_);
		entity_.m_pScene->m_ecs.addEntityComponent<LightComponent::Params>(entity_);
		Light::s_current_light_count++;

		//VOG_CORE_LOG_INFO("current light count: {0}", Light::s_current_light_count);
	}

	template<> void Scene::_onComponentAdded(Entity entity_, NativeScriptComponent& component_)
	{
	}

	template<> void Scene::_onComponentAdded(Entity entity_, IDComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, TagComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, TransformComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, MaterialComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, MeshComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, MeshRendererComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, AnimationComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, BoneTransformComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, SpriteRendererComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, BoxColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, CapsuleColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, SphereColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, PlaneColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, AABBColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, OBBColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, TriangleColliderComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, ParticleSystemComponent& component_) {}
	template<> void Scene::_onComponentAdded(Entity entity_, TestComponent& component_) {}
#pragma endregion

#pragma region onComponentRemove

	template<typename T>
	void Scene::_onComponentRemoved(Entity entity_)
	{
		VOG_CORE_LOG_INFO("T onRemove!");
		assert(0);
	}

	template<> void Scene::_onComponentRemoved<LightComponent>(Entity entity_)
	{
		entity_.m_pScene->m_ecs.removeEntityComponent<LightComponent::Type>(entity_);
		entity_.m_pScene->m_ecs.removeEntityComponent<LightComponent::Position>(entity_);
		entity_.m_pScene->m_ecs.removeEntityComponent<LightComponent::Direction>(entity_);
		entity_.m_pScene->m_ecs.removeEntityComponent<LightComponent::Color>(entity_);
		entity_.m_pScene->m_ecs.removeEntityComponent<LightComponent::Params>(entity_);

		Light::s_current_light_count--;
		//VOG_CORE_LOG_INFO("current light count: {0}", Light::s_current_light_count);
	}

	template<> void Scene::_onComponentRemoved<IDComponent>						(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<TagComponent>					(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<TransformComponent>				(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<MaterialComponent>				(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<MeshComponent>					(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<MeshRendererComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<AnimationComponent>				(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<BoneTransformComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<SpriteRendererComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<NativeScriptComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<BoxColliderComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<CapsuleColliderComponent>		(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<SphereColliderComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<PlaneColliderComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<AABBColliderComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<OBBColliderComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<TriangleColliderComponent>		(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<ParticleSystemComponent>			(Entity entity_) {}
	template<> void Scene::_onComponentRemoved<TestComponent>					(Entity entity_) {}
#pragma endregion

}