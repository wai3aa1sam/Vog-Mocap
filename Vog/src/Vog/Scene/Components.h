#pragma once
#include "Vog/Core/Core.h"
#include "Vog/Core/UUID.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Graphics/Buffer.h"
#include "Vog/Graphics/Mesh.h"
#include "Vog/Graphics/Material.h"

#include "Vog/Objects/Light.h"

#include "Vog/Physics/Test/PhysicsEngine.h"	// TODO: remove test

#include "Vog/Scene/NativeScriptEntity.h"

#include <memory>
#include <type_traits>

namespace vog {

	class Entity;			// TODO:: removev
	class NativeScriptEntity;
	struct NativeScriptComponent;

	struct IDComponent
	{
		UUID id;
		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id_)
			:
			id(id_)
		{}
	};

	struct TagComponent
	{
		std::string tag = "";

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag_)
			:
			tag(tag_)
		{}
	};

	struct TransformComponent
	{
		Vector3f translation = { 0.0f, 0.0f, 0.0f };
		Vector3f rotation = { 0.0f, 0.0f, 0.0f };
		Vector3f scale = { 1.0f, 1.0f, 1.0f };

		Matrix4f worldMatrix = Matrix4f(1.0f);
		bool isDirty = false;					// TODO: implement with isDirty for optimization
		bool isEnable = true;					// TODO: remove

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation_)
			: translation(translation_) {}

		Matrix4f getTransform() const
		{
			return MyMath::translate(translation)
				* MyMath::toMatrix4f(rotation)
				* MyMath::scale(scale);
		}

		void updateWorldMatrix()
		{
			worldMatrix = getTransform();
		}

		void updateWorldMatrix(const Matrix4f& parentMatrix_)
		{
			worldMatrix = parentMatrix_ * getTransform();
		}
	};

	struct MaterialComponent
	{
		RefPtr<Material> pMaterial;
		MaterialComponent() = default;
		MaterialComponent(const MaterialComponent&) = default;
		MaterialComponent(const RefPtr<Material>& pMaterial_)
			: pMaterial(pMaterial_) {}

	};

	struct MeshComponent
	{
		// TODO: revisit
		RefPtr<Model> pModel;

		RefPtr<VertexBuffer> pVertexBuffer;
		RefPtr<IndexBuffer> pIndexBuffer;

		uint32_t indexCount = 0;
		// temp

		bool isAnimated = false;		// TODO: the vertices should be struct of array instead of putting together

		std::vector<MeshVertexLayout> vertices;
		std::vector<MeshAnimatedVertexLayout> animatedVertices;

		std::vector<IndexType> indices;

		SubMesh subMesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		~MeshComponent() = default;

		void set(const RefPtr<Model>& pModel_)
		{
			pModel = pModel_;
			pVertexBuffer = pModel->m_pVertexBuffer;
			pIndexBuffer = pModel->m_pIndexBuffer;
		}
	};

	struct MeshRendererComponent
	{
		RefPtr<Material> pMaterial;

		MeshRendererComponent(const RefPtr<Shader>& pShader_)
			:
			pMaterial(Material::create(pShader_))
		{
		}
		MeshRendererComponent() = default;
		MeshRendererComponent(const MeshRendererComponent& rhs_) = default;
		//MeshRendererComponent(const MeshRendererComponent& rhs_)				// this will create many unnecessary copy!, but solve the copy problem
		//	:
		//	pMaterial(Material::create(rhs_.pMaterial))
		//{
		//}
		~MeshRendererComponent() = default;
	};

	struct LightComponent
	{
		struct Type
		{
			LightType type;
			Type() = default;
			Type(const Type&) = default;
			~Type() = default;
		};
		struct Position
		{
			Vector4f posiiton;
			Position()
			{
				//VOG_CORE_LOG_TRACE("Light position create!");
			}
			Position(const Position&) = default;
			//~Position() = default;
			~Position()
			{
				//VOG_CORE_LOG_TRACE("Light position destroy!");
			}

		};
		struct Direction
		{
			Vector3f direction;
			Direction()
			{
				//VOG_CORE_LOG_TRACE("Light direction create!");
			}
			Direction(const Direction&) = default;
			//~Position() = default;
			~Direction()
			{
				//VOG_CORE_LOG_TRACE("Light direction destroy!");
			}
		};
		struct Color
		{
			Vector4f color = { 1.0f, 1.0f, 1.0f, 1.0f };
			Color() = default;
			Color(const Color&) = default;
			~Color() = default;
		};
		struct Params
		{
			Vector4f params = { 1.0f, 1.0f, 1.0f, 1.0f };
			Params() = default;
			Params(const Params&) = default;
			~Params() = default;
		};

		LightComponent() = default;
		LightComponent(const LightComponent&) = default;
		~LightComponent() = default;
	};

	struct AnimationComponent		// TODO: revist
	{
		RefPtr<Model> pModel;

		AnimationComponent() = default;
		AnimationComponent(const AnimationComponent&) = default;
		~AnimationComponent() = default;

		//std::unordered_map<std::string, BoneInfo> boneInfoMap;
		//std::vector<Matrix4f> boneTransforms;
		//float duration = 0.0f;
		//float ticksPerSecond = 0.0f;
		//float currentTime = 0.0f;
		//float deltaTime = 0.0f;
	};
	
	// TODO: remove
	struct BoneTransformComponent
	{
		Transform* pBoneTransform = nullptr;

		BoneTransformComponent() = default;
		BoneTransformComponent(const BoneTransformComponent&) = default;
		~BoneTransformComponent() = default;
	};

	// TODO: remove
	struct SpriteRendererComponent
	{
		Transform* pTransform = nullptr;
		RefPtr<Texture> pTexture;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		~SpriteRendererComponent() = default;
	};

	struct NativeScriptComponent
	{
		NativeScriptEntity* pInstance = nullptr;
		NativeScriptEntity* (*pInstantiateFunc)(void) = nullptr;

		void (NativeScriptEntity::* pOnTriggerEnterFunc)(void) = &NativeScriptEntity::onTriggerEnter;

		void* pUeserData = nullptr;			// TODO: must remove, just temporary solve for the demo games

		template<typename T>
		void bind()
		{
			pInstantiateFunc = []() { return static_cast<NativeScriptEntity*>(new T()); };
			//VOG_CORE_ASSERT(std::is_base_of_v<NativeScriptEntity, T>, "");
			//pInstance = new T();
		}

		template<typename T>
		T* get()
		{
			return dynamic_cast<T*>(pInstance);
		}

		NativeScriptComponent() = default;
		//NativeScriptComponent() = default;
		~NativeScriptComponent()
		{
			//VOG_CORE_LOG_TRACE("NativeScriptComponent destructor");

			if (pInstance)
			{
				delete pInstance;
				pInstance = nullptr;
				pUeserData = nullptr;
			}
		}
		//NativeScriptComponent(const NativeScriptComponent&) = default;

		//NativeScriptComponent(const NativeScriptComponent& rhs_) = delete;
		//void operator=(const NativeScriptComponent& rhs_) = delete;

#if 1
		NativeScriptComponent(const NativeScriptComponent& rhs_)
		{
			//VOG_CORE_LOG_TRACE("NativeScriptComponent copy");
			//pInstance = rhs_.pInstance;
			//rhs_.pInstance = nullptr;

			pInstantiateFunc = rhs_.pInstantiateFunc;

			pUeserData = rhs_.pUeserData;
		}
		void operator=(const NativeScriptComponent& rhs_)
		{
			//VOG_CORE_LOG_TRACE("NativeScriptComponent copy operator");
			if (this == &rhs_)
				return;
			if (pInstance)
				delete pInstance;
			//pInstance = rhs_.pInstance;
			//rhs_.pInstance = nullptr;

			pInstantiateFunc = rhs_.pInstantiateFunc;

			pUeserData = rhs_.pUeserData;
		}
#endif // 0

		NativeScriptComponent(NativeScriptComponent&& rhs_)
		{
			//VOG_CORE_LOG_TRACE("NativeScriptComponent move");
			pInstance = rhs_.pInstance;
			pInstantiateFunc = rhs_.pInstantiateFunc;
			pUeserData = rhs_.pUeserData;

			rhs_.pInstance = nullptr;
			rhs_.pInstantiateFunc = nullptr;
			rhs_.pUeserData = nullptr;
		}
		void operator=(NativeScriptComponent&& rhs_)
		{
			//VOG_CORE_LOG_TRACE("NativeScriptComponent move operator");
			if (this == &rhs_)
				return;
			if (pInstance)
				delete pInstance;

			pInstance = rhs_.pInstance;
			pInstantiateFunc = rhs_.pInstantiateFunc;
			pUeserData = rhs_.pUeserData;

			rhs_.pInstance = nullptr;
			rhs_.pInstantiateFunc = nullptr;
			rhs_.pUeserData = nullptr;
		}
	};


#pragma region Physics_stuff

	struct RigidbodyComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType type = BodyType::Static;
		bool fixedRotation = false;

		Vector3b isFreezePosiiton;
		Vector3b isFreezeRotation;

		Vector3f velocity = { 0.0f, 0.0f, 0.0f };

		// Storage for runtime
		void* pRuntimeBody = nullptr;

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;
	};

	struct BoxColliderComponent
	{
		bool isTrigger = false;

		Vector3f offset = { 0.0f, 0.0f, 0.0f };
		Vector3f size = { 0.5f, 0.5f, 0.5f };

		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;
	};

	struct CapsuleColliderComponent
	{
		bool isTrigger = false;

		Vector3f center = { 0.0f, 0.0f, 0.0f };
		float height = 2.0f;
		float radius = 0.5f;

		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent&) = default;
	};

	struct SphereColliderComponent
	{
		bool isTrigger = false;

		Vector3f center = { 0.0f, 0.0f, 0.0f };
		float radius = 0.5f;

		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;
	};

	struct PlaneColliderComponent
	{
		bool isTrigger = false;

		// Storage for runtime
		void* pRuntimeShape = nullptr;

		PlaneColliderComponent() = default;
		PlaneColliderComponent(const PlaneColliderComponent&) = default;
	};

	struct AABBColliderComponent		// TODO: remove temp
	{
		bool isTrigger = false;

		Vector3f center = { 0.0f, 0.0f, 0.0f };
		Vector3f size = { 0.5f, 0.5f, 0.5f };

		AABBColliderComponent() = default;
		AABBColliderComponent(const AABBColliderComponent&) = default;
	};

	struct OBBColliderComponent			// TODO: remove temp
	{
		bool isTrigger = false;

		Vector3f center = { 0.0f, 0.0f, 0.0f };
		Vector3f size = { 0.5f, 0.5f, 0.5f };

		OBBColliderComponent() = default;
		OBBColliderComponent(const OBBColliderComponent&) = default;
	};

	struct TriangleColliderComponent			// TODO: remove temp
	{
		bool isTrigger = false;

		TriangleColliderComponent() = default;
		TriangleColliderComponent(const TriangleColliderComponent&) = default;
	};
#pragma endregion

	// TODO: remove
	struct ParticleSystemComponent
	{
		void* pParticles = nullptr;

		struct  ParticleVertexLayout
		{
			Vector3f position;
		};
		ParticleVertexLayout* pParticlesBuffer = nullptr;
		ParticleVertexLayout* itParticlesBuffer = nullptr;
		int particlesCount = 0;
		RefPtr<VertexBuffer> pParticleVertexBuffer;
		//RefPtr<VertexArray> pParticleVertexArray;

		ParticleSystemComponent()
		{
			pParticlesBuffer = new ParticleVertexLayout[Test::PhysicsEngine::s_max_particles_count];
			itParticlesBuffer = pParticlesBuffer;

			pParticleVertexBuffer = VertexBuffer::create(pParticlesBuffer, sizeof(pParticlesBuffer[0]) * Test::PhysicsEngine::s_max_particles_count);
			VertexLayout layout = {
			{ VertexDataType::Float3, "a_position",	},
			};
			pParticleVertexBuffer->setVertexLayout(layout);
		}

		ParticleSystemComponent(const ParticleSystemComponent& rhs_)
		{
			pParticlesBuffer = new ParticleVertexLayout[Test::PhysicsEngine::s_max_particles_count];
			itParticlesBuffer = pParticlesBuffer;

			pParticleVertexBuffer = VertexBuffer::create(pParticlesBuffer, sizeof(pParticlesBuffer[0]) * Test::PhysicsEngine::s_max_particles_count);
			VertexLayout layout = {
			{ VertexDataType::Float3, "a_position",	},
			};
			pParticleVertexBuffer->setVertexLayout(layout);

			particlesCount = rhs_.particlesCount;
			pParticles = rhs_.pParticles;
		}

		void operator=(const ParticleSystemComponent& rhs_)
		{
			pParticlesBuffer = new ParticleVertexLayout[Test::PhysicsEngine::s_max_particles_count];
			itParticlesBuffer = pParticlesBuffer;

			pParticleVertexBuffer = VertexBuffer::create(pParticlesBuffer, sizeof(pParticlesBuffer[0]) * Test::PhysicsEngine::s_max_particles_count);
			VertexLayout layout = {
			{ VertexDataType::Float3, "a_position",	},
			};
			pParticleVertexBuffer->setVertexLayout(layout);

			particlesCount = rhs_.particlesCount;
			pParticles = rhs_.pParticles;
		}

		ParticleSystemComponent(ParticleSystemComponent&& rhs_)
		{
			if (pParticlesBuffer)
			{
				delete[] pParticlesBuffer;
				pParticlesBuffer = nullptr;
				itParticlesBuffer = nullptr;
			}

			pParticlesBuffer = rhs_.pParticlesBuffer;
			itParticlesBuffer = rhs_.pParticlesBuffer;
			pParticleVertexBuffer = rhs_.pParticleVertexBuffer;
			particlesCount = rhs_.particlesCount;
			pParticles = rhs_.pParticles;
		}
		void operator=(ParticleSystemComponent&& rhs_)
		{
			if (this == &rhs_)
				return;
			if (pParticlesBuffer)
			{
				delete[] pParticlesBuffer;
				pParticlesBuffer = nullptr;
				itParticlesBuffer = nullptr;
			}

			pParticlesBuffer = rhs_.pParticlesBuffer;
			itParticlesBuffer = rhs_.pParticlesBuffer;
			pParticleVertexBuffer = rhs_.pParticleVertexBuffer;
			particlesCount = rhs_.particlesCount;
			pParticles = rhs_.pParticles;
		}

		~ParticleSystemComponent()
		{
			if (pParticlesBuffer)
			{
				delete[] pParticlesBuffer;
				pParticlesBuffer = nullptr;
			}
		}
	};

	struct TestComponent
	{
		int* p = nullptr;
		std::unique_ptr<int> up;

		TestComponent()
		{
			//VOG_CORE_LOG_TRACE("TestComponent ctor");
			p = new int[30];
			up = std::make_unique<int>();
		}
		TestComponent(const TestComponent& rhs_)
		{
			//VOG_CORE_LOG_TRACE("TestComponent copy");
			p = new int[30];
			up = std::make_unique<int>();
		}
		void operator=(const TestComponent& rhs_)
		{
			//VOG_CORE_LOG_TRACE("TestComponent copy");
			p = new int[30];
			up = std::make_unique<int>();
		}
		TestComponent(TestComponent&& rhs_)
		{
			//VOG_CORE_LOG_TRACE("TestComponent move");
			p = rhs_.p;
			rhs_.p = nullptr;
			up.reset(rhs_.up.release());
		}
		void operator=(TestComponent&& rhs_)
		{
			//VOG_CORE_LOG_TRACE("TestComponent move");
			if (this == &rhs_)
				return;
			if (p)
				delete[] p;

			p = rhs_.p;
			rhs_.p = nullptr;
			up.reset(rhs_.up.release());
		}
		~TestComponent()
		{
			//VOG_CORE_LOG_TRACE("TestComponent dtor");
			delete[] p;
		}
	};
}