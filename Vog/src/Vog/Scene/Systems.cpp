#include "vogpch.h"
#include "Systems.h"

#include "Components.h"
#include "Entity.h"
#include "NativeScriptEntity.h"
#include "Scene.h"
#include "ECS.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/Graphics/Renderer/DeferredRenderer.h"
#include "Vog/Graphics/Renderer/Renderer.h"

#include "Vog/Physics/Physics.h"

namespace vog {

	void TransformSystem::update(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();
		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& transforms = transform_cm.getComponents();

		for (size_t i = 0; i < transforms.size(); i++)
		{
			transforms[i].updateWorldMatrix();
		}
	}

#pragma region TransformHierarchySystem
	static void _updateTransformMatrix(Matrix4f& parent_parentMatrix,
										const ecs::HierarchyComponent& parentHierarchy_, TransformComponent& parentTransform_,
										ecs::ComponentManager<ecs::HierarchyComponent>& hierachy_cm_, ecs::ComponentManager<TransformComponent>& transform_cm_)
	{
		using namespace ecs;

		parentTransform_.updateWorldMatrix(parent_parentMatrix);

		for (size_t i = 0; i < parentHierarchy_.children.size(); i++)		// MUST BE hierachy.children.size()!
		{
			EntityHandle childHandle = parentHierarchy_.children[i];
			auto& childTransform = *transform_cm_.getpComponent(childHandle);						//should check nullptr
			const HierarchyComponent& child_hier = *hierachy_cm_.getpComponent(childHandle);		//should check nullptr

			VOG_CORE_ASSERT(transform_cm_.getpComponent(childHandle), "");
			VOG_CORE_ASSERT(hierachy_cm_.getpComponent(childHandle), "");

			_updateTransformMatrix(parentTransform_.worldMatrix, child_hier, childTransform, hierachy_cm_, transform_cm_);
		}
	}

	void TransformHierarchySystem::update(Scene* pScene_)
	{
		VOG_CORE_ASSERT(pScene_, "");

		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& hierarchy_cm = ecs.getComponentManager<HierarchyComponent>();
		auto& transform_cm = ecs.getComponentManager<TransformComponent>();

		const auto& hierarchies = hierarchy_cm.getComponents();
		auto& transforms = transform_cm.getComponents();

		for (size_t i = 0; i < hierarchies.size(); i++)
		{
			const HierarchyComponent& parent_hier = hierarchies[i];
			auto parent_handle = parent_hier.parentID;

			if (parent_handle == s_invalid_entity)							// root
			{
				EntityHandle actualHandle = hierarchy_cm.getEntityHandle(i);
				auto& parentTransform = *transform_cm.getpComponent(actualHandle);		// should check nullptr

				VOG_CORE_ASSERT(transform_cm.getpComponent(actualHandle), "");

				_updateTransformMatrix(Matrix4f(1.0f), parent_hier, parentTransform, hierarchy_cm, transform_cm);
			}
		}
	}
#pragma endregion

	
	void MeshRendererSystem::uploadLights(Scene* pScene_)
	{
		auto& ecs = pScene_->getECS();

		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& light_cm = ecs.getComponentManager<LightComponent>();
		auto& light_type_cm = ecs.getComponentManager<LightComponent::Type>();
		auto& light_position_cm = ecs.getComponentManager<LightComponent::Position>();
		auto& light_direction_cm = ecs.getComponentManager<LightComponent::Direction>();
		auto& light_color_cm = ecs.getComponentManager<LightComponent::Color>();
		auto& light_params_cm = ecs.getComponentManager<LightComponent::Params>();

		VOG_CORE_ASSERT(light_cm.getCount() == light_position_cm.getCount() 
						&& light_cm.getCount() == light_color_cm.getCount() 
						&& light_cm.getCount() == light_params_cm.getCount(), "");

		for (size_t i = 0; i < light_cm.getCount(); i++)
		{
			auto entityHandle = light_cm.getEntityHandle(i);
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);
			LightType type = light_type_cm.getComponent_unsafe(entityHandle).type;
			auto& params = light_params_cm.getComponent_unsafe(entityHandle).params;
			params.y = 0.0f;

			if (type == LightType::Point)
			{
				light_position_cm.getComponent_unsafe(entityHandle).posiiton = Vector4f(transform.translation, 1.0f);
				//light_direction_cm.getComponent_unsafe(entityHandle).direction = MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 0.0f, 1.0f);
			}
			else if (type == LightType::Directional)
			{
				light_position_cm.getComponent_unsafe(entityHandle).posiiton = Vector4f(transform.translation, 0.0f);
				light_direction_cm.getComponent_unsafe(entityHandle).direction = MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 0.0f, 1.0f);
				//light_position_cm.getComponent_unsafe(entityHandle).posiiton = Vector4f(MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 0.0f, 1.0f), 0.0f);
			}
			else if (type == LightType::Spot)
			{
				light_position_cm.getComponent_unsafe(entityHandle).posiiton = Vector4f(transform.translation, 1.0f);
				light_direction_cm.getComponent_unsafe(entityHandle).direction = MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 0.0f, 1.0f);
				params.y = 1.0f;
			}
			else
				VOG_CORE_ASSERT(0, "");
		}

		DeferredRenderer::uploadLights(light_position_cm.getData(), light_direction_cm.getData(), light_color_cm.getData(), light_params_cm.getData());

		// Light Space for shadow mapping
		{
			auto entityHandle = light_cm.getEntityHandle(0);
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);
			const Vector3f& light_position = transform.translation;
			auto forward = MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 0.0f, 1.0f);
			DeferredRenderer::uploadLighViewMatrix(MyMath::lookAt(light_position, light_position + forward, Vector3f(0.0f, 1.0f, 0.0f)));
		}
	}

	void MeshRendererSystem::update(Scene* pScene_)
	{
		auto& ecs = pScene_->getECS();

		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& mesh_cm = ecs.getComponentManager<MeshComponent>();
		auto& meshRenderer_cm = ecs.getComponentManager<MeshRendererComponent>();

		auto& meshRendererComponents = meshRenderer_cm.getComponents();

		//VOG_CORE_ASSERT(meshRenderer_cm.getCount() == mesh_cm.getCount(), "");

		for (size_t i = 0; i < meshRenderer_cm.getCount(); i++)
		{
			auto entity = meshRenderer_cm.getEntityHandle(i);

			auto& transform = transform_cm.getComponent_unsafe(entity);
			auto& mesh = mesh_cm.getComponent_unsafe(entity);
			auto& meshRenderer = meshRenderer_cm.getComponent_unsafe(entity);

			if (!transform.isEnable)
				continue;

			auto& pShader = meshRenderer.pMaterial->getShader();

			if (mesh.isAnimated && AssetManager::isAnimatedShader(pShader))
			{
				AnimatedSubmission submission;

				submission.pVertexBuffer = mesh.pVertexBuffer;
				submission.pIndexBuffer = mesh.pIndexBuffer;
				submission.pMaterial = meshRenderer.pMaterial;
				submission.pTransform = &transform.worldMatrix;
				submission.subMesh = mesh.subMesh;
				submission.pBoneTransforms = &mesh.pModel->getBoneTransforms();

				submission.indexCount = mesh.indexCount;

				DeferredRenderer::submit(std::move(submission));
			}
			else
			{
				Submission submission;

				submission.pVertexBuffer = mesh.pVertexBuffer;
				submission.pIndexBuffer = mesh.pIndexBuffer;
				submission.pMaterial = meshRenderer.pMaterial;
				submission.pTransform = &transform.worldMatrix;
				submission.subMesh = mesh.subMesh;

				submission.indexCount = mesh.indexCount;

				DeferredRenderer::submit(std::move(submission));
			}
			//DeferredRenderer::submit(pVertexBuffer, pIndexBuffer, transform.worldMatrix, meshRenderer.pMaterial);
		}
	}

	void AnimationSystem::update(float timestep_, Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& animation_cm = ecs.getComponentManager<AnimationComponent>();
		auto& animationComponents = animation_cm.getComponents();

		for (size_t i = 0; i < animation_cm.getCount(); i++)
		{
			animationComponents[i].pModel->onUpdateAnimation(timestep_);
		}
	}

	void BoneTransformSystem::update(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& boneTransform_cm = ecs.getComponentManager<BoneTransformComponent>();
		auto& boneTransformComponents = boneTransform_cm.getComponents();

		for (size_t i = 0; i < boneTransformComponents.size(); i++)
		{
			boneTransformComponents[i].pBoneTransform->updateTransformation();
		}
	}

#pragma region Physics

	void PhysicsSystem::onStart(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		pScene_->m_pPhysicsWorld = new PhysicsWorld;

		auto& rigidbody_cm = ecs.getComponentManager<RigidbodyComponent>();
		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& nativeScript_cm = ecs.getComponentManager<NativeScriptComponent>();

		for (size_t i = 0; i < rigidbody_cm.getCount(); i++)
		{
			Entity entity = { rigidbody_cm.getEntityHandle(i), pScene_ };

			auto& rigidbody = rigidbody_cm.getComponents()[i];
			auto& transform = transform_cm.getComponent_unsafe(entity);

			auto* pBody = pScene_->m_pPhysicsWorld->addBody();

			//pBody->m_pUserData = static_cast<const EntityHandle*>(entity); // return local pointer will gg
			pBody->m_userHandle = entity;		// TODO: remove temp

			rigidbody.pRuntimeBody = pBody;

			Vector3f translation;
			Vector3f scale;
			Vector3f rotation;
			MyMath::decompose(transform.worldMatrix, scale, rotation, translation);
			pBody->m_transform.position = translation;
			pBody->m_transform.orientation = MyMath::toQuaternion(rotation);

			/*pBody->m_transform.position = transform.translation;
			pBody->m_transform.orientation = MyMath::toQuaternion(transform.rotation);*/
			pBody->m_type = (BodyType)rigidbody.type;

			if (auto* pNativeScript = nativeScript_cm.getpComponent(entity))		// should be no resize problem
			{
				VOG_CORE_ASSERT(pNativeScript->pInstance, "");
				pBody->m_pCollisionCallbackBase = (CollisionCallbackBase*)pNativeScript->pInstance;
			}

			if (entity.hasComponent<CapsuleColliderComponent>())
			{
				auto& capsuleCollider = entity.getComponent<CapsuleColliderComponent>();

				auto height = capsuleCollider.height * transform.scale.y;
				auto normal = MyMath::eulerToMatrix3f(transform.rotation) * Vector3f(0.0f, 1.0f, 0.0f);
				auto radius = MyMath::max(transform.scale.x, transform.scale.z) * capsuleCollider.radius;

				pBody->setAsCapsule(MyMath::toQuaternion(rotation) * capsuleCollider.center, normal, height, radius);
			}
			else if (entity.hasComponent<SphereColliderComponent>())
			{
				auto& sphereCollider = entity.getComponent<SphereColliderComponent>();

				//SphereShape* pSphere = new SphereShape(sphereCollider.center, (sphereCollider.radius * 2.0f) *
				//								MyMath::max(transform.scale.x, MyMath::max(transform.scale.y, transform.scale.z)));		// TODO: rectify
				//pBody->m_pShape = pSphere;

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

				//OBBShape* pOBB = new OBBShape(obbCollider.center, obbCollider.size * transform.scale * 2.0f, transform.rotation);
				//pBody->m_pShape = pOBB;

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

		pScene_->m_pPhysicsWorld->onStart();		// TODO: remove
	}

	void PhysicsSystem::uploadData(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& transform_cm = ecs.getComponentManager<TransformComponent>();
		auto& rigidbody_cm = ecs.getComponentManager<RigidbodyComponent>();
		for (size_t i = 0; i < rigidbody_cm.getCount(); i++)
		{
			Entity entity = { rigidbody_cm.getEntityHandle(i), pScene_ };

			auto& rigidbody = rigidbody_cm.getComponents()[i];
			auto& transform = transform_cm.getComponent_unsafe(entity);

			if (rigidbody.pRuntimeBody)
			{
				VOG_CORE_ASSERT(rigidbody.pRuntimeBody, "");

				Body* pBody = reinterpret_cast<Body*>(rigidbody.pRuntimeBody);

				pBody->m_isEnable = transform.isEnable;

				Vector3f translation;
				Vector3f scale;
				Vector3f rotation;
				MyMath::decompose(transform.worldMatrix, scale, rotation, translation);
				pBody->m_transform.position = translation;
				pBody->m_transform.orientation = MyMath::toQuaternion(rotation);

				pBody->m_type = (BodyType)rigidbody.type;

				pBody->m_data.velocity = rigidbody.velocity;
			}
		}
	}

	void PhysicsSystem::update(PhysicsWorld* pPhysicsWorld_, float timestep_, Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		pPhysicsWorld_->step(timestep_);

		auto& rigidbody_cm = ecs.getComponentManager<RigidbodyComponent>();
		auto& transform_cm = ecs.getComponentManager<TransformComponent>();

		for (size_t i = 0; i < rigidbody_cm.getCount(); i++)
		{
			Entity entity = { rigidbody_cm.getEntityHandle(i), pScene_ };

			auto& rigidbody = rigidbody_cm.getComponents()[i];
			auto& transform = transform_cm.getComponent_unsafe(entity);

			if (!transform.isEnable)
				continue;

			if (rigidbody.pRuntimeBody)
			{
				Body* pBody = reinterpret_cast<Body*>(rigidbody.pRuntimeBody);

				// temp, world to local coordinate
				Vector3f translation;
				Vector3f scale;
				Vector3f rotation;
				MyMath::decompose(transform.worldMatrix, scale, rotation, translation);

				Vector3f relative_translation = translation - transform.translation;
				Vector3f relative_rotation = rotation - transform.rotation;

				auto final_rotation = MyMath::toEulerAngles(pBody->m_transform.orientation) - relative_rotation;

				//auto phyiscs_world_rotation = MyMath::toEulerAngles(MyMath::normalize(pBody->m_transform.orientation));
				auto phyiscs_world_rotation = MyMath::toEulerAngles(pBody->m_transform.orientation);

				transform.translation = pBody->m_transform.position - relative_translation;
				transform.rotation = phyiscs_world_rotation - relative_rotation;
			}
		}
	}
#pragma endregion


#pragma region NativeScript
	void NativeScriptSystem::onAwake(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& nativeScript_cm = ecs.getComponentManager<NativeScriptComponent>();
		for (size_t i = 0; i < nativeScript_cm.getCount(); i++)
		{
			auto& nativeScript = nativeScript_cm.getComponents()[i];

			VOG_CORE_ASSERT(nativeScript.pInstantiateFunc, "");
			VOG_CORE_ASSERT(!nativeScript.pInstance, "");

			nativeScript.pInstance = nativeScript.pInstantiateFunc();
			nativeScript.pInstance->m_entity = { nativeScript_cm.getEntityHandle(i), pScene_ };
			nativeScript.pInstance->onAwake();
		}
	}

	void NativeScriptSystem::onStart(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& nativeScript_cm = ecs.getComponentManager<NativeScriptComponent>();
		for (size_t i = 0; i < nativeScript_cm.getCount(); i++)
		{
			auto& nativeScript = nativeScript_cm.getComponents()[i];

			VOG_CORE_ASSERT(nativeScript.pInstantiateFunc, "");
			VOG_CORE_ASSERT(nativeScript.pInstance, "");

			nativeScript.pInstance->onStart();
		}
	}

	void NativeScriptSystem::update(Scene* pScene_, float timestep_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& nativeScript_cm = ecs.getComponentManager<NativeScriptComponent>();
		for (size_t i = 0; i < nativeScript_cm.getCount(); i++)
		{
			auto& nativeScript = nativeScript_cm.getComponents()[i];

			VOG_CORE_ASSERT(nativeScript.pInstantiateFunc, "");
			VOG_CORE_ASSERT(nativeScript.pInstance, "");

			nativeScript.pInstance->onUpdate(timestep_);
		}
	}

	void NativeScriptSystem::onDestroy(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& nativeScript_cm = ecs.getComponentManager<NativeScriptComponent>();
		for (size_t i = 0; i < nativeScript_cm.getCount(); i++)
		{
			auto& nativeScript = nativeScript_cm.getComponents()[i];

			VOG_CORE_ASSERT(nativeScript.pInstantiateFunc, "");
			VOG_CORE_ASSERT(nativeScript.pInstance, "");

			nativeScript.pInstance->onDestroy();
		}
	}
#pragma endregion

#pragma region Test
	void TestPhysicsSystem::update(Test::PhysicsEngine* pPhysicsEngine_, float timestep_, Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		pPhysicsEngine_->step(timestep_);
		auto& particleSystem_cm = ecs.getComponentManager<ParticleSystemComponent>();
		auto& particleSystemComponents = particleSystem_cm.getComponents();

		for (size_t i = 0; i < particleSystem_cm.getCount(); i++)
		{
			auto& particleSystem = particleSystemComponents[i];
			auto* particles = static_cast<Test::Particle*>(particleSystem.pParticles);

			for (size_t i = 0; i < particleSystem.particlesCount; i++)
			{
				particleSystem.pParticlesBuffer[i].position = particles[i].position;
				//VOG_CORE_LOG_GLM(particleSystem.pParticlesBuffer[i].position);
			}
			//VOG_CORE_LOG_INFO("size: {0}", sizeof(particleSystem.pParticlesBuffer[0]) * particleSystem.particlesCount);
			particleSystem.pParticleVertexBuffer->setData(particleSystem.pParticlesBuffer, sizeof(particleSystem.pParticlesBuffer[0]) * particleSystem.particlesCount);
		}
	}

	void TestRenderSystem::update(Scene* pScene_)
	{
		using namespace ecs;
		auto& ecs = pScene_->getECS();

		auto& transform_cm = ecs.getComponentManager<TransformComponent>();

		// test
		//for (auto& transform: transform_cm.getComponents())
		//{
		//	Renderer::drawQuad(transform.worldMatrix);
		//}

		auto& spirte_cm = ecs.getComponentManager<SpriteRendererComponent>();

		for (size_t i = 0; i < spirte_cm.getCount(); i++)
		{
			auto entity = spirte_cm.getEntityHandle(i);
			auto& sprite = spirte_cm.getComponents()[i];
			//Renderer::drawCube(transform_cm.getComponent_unsafe(entity).worldMatrix, sprite.pTexture);
		}

		// TODO: remove test
		auto& particleSystem_cm = ecs.getComponentManager<ParticleSystemComponent>();
		auto& particleSystemComponents = particleSystem_cm.getComponents();
		for (size_t i = 0; i < particleSystem_cm.getCount(); i++)
		{
			auto entity = particleSystem_cm.getEntityHandle(i);

			auto& particleSystem = particleSystemComponents[i];

			VOG_CORE_ASSERT(particleSystem.particlesCount <= Test::PhysicsEngine::s_max_particles_count, "");
			if (particleSystem.particlesCount)
			{
				Renderer::drawPoints(particleSystem.pParticleVertexBuffer, particleSystem.particlesCount, transform_cm.getComponent_unsafe(entity).worldMatrix);
			}
		}
	}
#pragma endregion
}