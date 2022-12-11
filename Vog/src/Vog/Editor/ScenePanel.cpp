#include "vogpch.h"
#include "ScenePanel.h"

#include "Vog/Scene/Components.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/ImGui/ImGuiLibrary.h"

#include "Vog/Physics/Physics.h"

#include <imgui.h>

namespace vog {

	ScenePanel::ScenePanel(const RefPtr<Scene>& pScene_)
		:
		m_pScene(pScene_)
	{
	}

	ScenePanel::~ScenePanel()
	{
	}

	void ScenePanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_pScene->getECS().for_each_rootEntity(
			[&](auto handle_)
			{
				Entity entity{ handle_, m_pScene.get()};
				_drawEntity(entity);
			}
		);
		
		// de selected entity if
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_selectedEntity = {};

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_pScene->createEntity("Empty Entity");

			if (ImGui::MenuItem("Create Light"))
				createLight();

			// Create primtives mesh
			{
				if (ImGui::MenuItem("Create AABB"))
					createAABB();

				if (ImGui::MenuItem("Create OBB"))
					createOBB();

				if (ImGui::MenuItem("Create Capsule"))
					createCapsule();

				if (ImGui::MenuItem("Create Sphere"))
					createSphere();

				if (ImGui::MenuItem("Create Plane"))
					createPlane();

				if (ImGui::MenuItem("Create Triangle"))
					createTriangle();
			}
			
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Inspector");

		if (m_selectedEntity)
		{
			_drawComponents(m_selectedEntity);
			_drawAddComponent();
		}
		ImGui::End();

		if (m_pScene->m_pPhysicsWorld)
		{
			m_pScene->m_pPhysicsWorld->onImGuiRender();
		}
	}

	void ScenePanel::setScene(const RefPtr<Scene>& pScene_)
	{
		m_pScene = pScene_;
		m_selectedEntity = {};
	}

	void ScenePanel::setSelectedEntity(Entity entity_)
	{
		m_selectedEntity = entity_;
	}

	void ScenePanel::_drawEntity(Entity entity_)
	{
		auto& id = entity_.getComponent<IDComponent>().id;
		auto& tag = entity_.getComponent<TagComponent>().tag;

		if (!entity_.getComponent<TransformComponent>().isEnable)		// TODO: remove, just for ui speed
		{
			return;
		}

		auto& hierachy = entity_.getComponent<ecs::HierarchyComponent>();

		ImGuiTreeNodeFlags flags = (m_selectedEntity == entity_ ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		flags |= (hierachy.children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0;

		std::string idString = std::to_string(entity_);

		bool isOpened = ImGui::TreeNodeEx((void*)(intptr_t)(uint32_t)entity_, flags, tag.c_str());
		if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))	// right or left click
			m_selectedEntity = entity_;

		//------------

		bool isEntityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_pScene->createEntity("Game Object", entity_);

			if (ImGui::MenuItem("Delete Entity"))
				isEntityDeleted = true;

			if (ImGui::MenuItem("Duplicate Entity"))
				m_pScene->duplicateEntity(m_selectedEntity, {});
			
			// Create primtives mesh
			{
				if (ImGui::MenuItem("Create AABB"))
					createAABB(m_selectedEntity);

				if (ImGui::MenuItem("Create OBB"))
					createOBB(m_selectedEntity);

				if (ImGui::MenuItem("Create Capsule"))
					createCapsule(m_selectedEntity);

				if (ImGui::MenuItem("Create Sphere"))
					createSphere(m_selectedEntity);

				if (ImGui::MenuItem("Create Plane"))
					createPlane(m_selectedEntity);

				if (ImGui::MenuItem("Create Triangle"))
					createTriangle(m_selectedEntity);
			}

			ImGui::EndPopup();
		}

		if (isOpened)		// recursive drawa if open the current node
		{
			for (size_t i = 0; i < hierachy.children.size(); i++)		// MUST BE hierachy.children.size()!
			{
				Entity entity{ hierachy.children[i], m_pScene.get()};
				_drawEntity(entity);
			}
			ImGui::TreePop();
		}

		if (isEntityDeleted)
		{
			m_pScene->destroyEntity_sorted(entity_);
			if (m_selectedEntity == entity_)
				m_selectedEntity = {};
		}
	}

	template<typename T, typename Func>
	static void drawComponent(const std::string& name_, Entity entity_, Func func_)
	{
		if (entity_.hasComponent<T>())
		{
			//ImGui::Text(name_.c_str());
			//auto& component = entity_.getComponent<T>();
			//func_(entity_, component);

			ImGui::PushID(entity_);

			/*auto boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			flags |= ImGuiTreeNodeFlags_DefaultOpen;

			bool isOpened = ImGui::TreeNodeEx(name_.c_str(), flags);
			if (isOpened)
			{
				ImGui::PopFont();

				auto& component = entity_.getComponent<T>();
				func_(entity_, component);
				ImGui::TreePop();
			}

			if (!isOpened)
			{
				ImGui::PopFont();
			}*/

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed 
				| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

			if (ImGui::CollapsingHeader(name_.c_str(), flags))
			{
				auto& component = entity_.getComponent<T>();
				func_(entity_, component);
			}

			ImGui::PopID();
		}
	}

	template<typename T>
	static void _addComponent(const std::string& name_, Entity entity_)
	{
		if (!entity_.hasComponent<T>())
		{
			if (ImGui::MenuItem(name_.c_str()))
			{
				entity_.addComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void ScenePanel::_drawAddComponent()
	{
		float buttonWidth = 50.0f * 4;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetWindowContentRegionMax().x - buttonWidth) / 2.0f);		// center align
		if (ImGui::Button("Add Component", ImVec2(buttonWidth, 25)))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			_addComponent<SpriteRendererComponent>("Sprite Renderer", m_selectedEntity);

			_addComponent<CapsuleColliderComponent>("Capsule Collider", m_selectedEntity);
			_addComponent<RigidbodyComponent>("RigidBody", m_selectedEntity);
			_addComponent<OBBColliderComponent>("OBB Collider", m_selectedEntity);
			_addComponent<SphereColliderComponent>("Sphere Collider", m_selectedEntity);
			_addComponent<PlaneColliderComponent>("Plane Collider", m_selectedEntity);


			_addComponent<LightComponent>("Light", m_selectedEntity);

			ImGui::EndPopup();
		}
	}

	void ScenePanel::_drawComponents(Entity entity_)
	{
		{	// Tag Component
			ImGui::Text("Tag");
			auto& component = entity_.getComponent<TagComponent>();
			auto& tag = component.tag;
			char buf[256];
			memset(buf, 0, sizeof(buf));
			strcpy_s(buf, sizeof(buf), tag.c_str());
			ImGui::SameLine();
			if (ImGui::InputText("##Tag: ", buf, sizeof(buf)))
			{
				tag = std::string(buf);
			}
		}

		drawComponent<TransformComponent>("Transform", entity_, 
			[](Entity entity_, TransformComponent& component_)
			{
				auto& transform = component_;

				ImGuiLibrary::drawVec3fControl("Position", transform.translation);
				Vector3f rotation = MyMath::degrees(transform.rotation);
				ImGuiLibrary::drawVec3fControl("Rotation", rotation);
				transform.rotation = MyMath::radians(rotation);
				ImGuiLibrary::drawVec3fControl("Scale", transform.scale, 1.0f);

				ImGuiLibrary::drawCheckbox("isEnable", transform.isEnable);

				//ImGui::NewLine();
			});

		drawComponent<MeshComponent>("Mesh", entity_,
			[](Entity entity_, MeshComponent& component_)
			{
				MeshComponent& mesh = component_;
				auto& pModel = mesh.pModel;
				auto& model_name = pModel->getName();

				ImGui::PushID(model_name.c_str());


				if (mesh.subMesh.nVertices > 0)
				{
					ImGuiLibrary::drawTextWithValue("Base Vertex Index: ", mesh.subMesh.baseVertexIndex);
					ImGuiLibrary::drawTextWithValue("Number of Vertices: ", mesh.subMesh.nVertices);
					ImGuiLibrary::drawTextWithValue("Base Index: ", mesh.subMesh.baseIndex);
					ImGuiLibrary::drawTextWithValue("Number of Indices: ", mesh.subMesh.nIndices);
					ImGuiLibrary::drawTextWithValue("Material Index: ", mesh.subMesh.materialIndex);
				}

				ImGui::Text("Mesh: ");
				ImGui::SameLine();
				if (ImGui::BeginCombo("", model_name.data()))
				{
					AssetManager::foreach_mesh([&](RefPtr<Model>& pModel_) {
						bool isSelected = model_name == pModel_->getName();				// should hash the string instead of compare str
						if (ImGui::Selectable(pModel_->getName().data(), isSelected))
						{
							// MeshComponent.reset(pModel_);
							//mesh.
							//pMaterial->resetShader(pShader_);
							if (mesh.subMesh.nVertices > 0)		// is a submesh
							{
								VOG_CORE_LOG_WARN("do not support to change a subMesh");
							}
							else
								mesh.set(pModel_);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
						});
					ImGui::EndCombo();
				}

				ImGui::PopID();

				//ImGui::NewLine();
			});

		drawComponent<MeshRendererComponent>("Mesh Renderer", entity_,
			[](Entity entity_, auto& component_)
			{
				MeshRendererComponent& meshRenderer = component_;
				auto& pMaterial = meshRenderer.pMaterial;
				auto& shaderName = pMaterial->getShader()->getName();
				auto shaderID = pMaterial->getShader()->getID();

				ImGui::PushID(shaderName.c_str());

				ImGuiLibrary::drawTextWithValue("Texture count", pMaterial->getTexturesCount());

				ImGui::Text("Shader: ");
				ImGui::SameLine();
				if (ImGui::BeginCombo("", shaderName.data()))
				{
					AssetManager::foreach_shader([&](RefPtr<Shader>& pShader_)		// TODO: consider to use vector instead of map for faster loop speed
						{
							bool isSelected = shaderID == pShader_->getID();
							if (ImGui::Selectable(pShader_->getName().data(), isSelected))
							{
								pMaterial->resetShader(pShader_);
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						});
					ImGui::EndCombo();
				}

				pMaterial->onImGuiRender();
				ImGui::PopID();
				//ImGui::NewLine();
			});

		drawComponent<LightComponent>("Light", entity_,
			[](Entity entity_, auto& component_)
			{
				LightComponent& light = component_;
				auto& type = entity_.getComponent<LightComponent::Type>().type;
				auto& color = entity_.getComponent<LightComponent::Color>().color;
				auto& params = entity_.getComponent<LightComponent::Params>().params;

				std::array<int, LightType::Count> type_ids = { 0, 1, 2, 3 };
				std::array<std::string, LightType::Count> type_name_str = { "Point", "Directional", "Spot", "Area" };
				static int id = 0;
				id++;

				ImGui::PushID(id);
				ImGui::Text("Type: ");
				ImGui::SameLine();
				if (ImGui::BeginCombo("", type_name_str[type].data()))
				{
					for (size_t i = 0; i < type_name_str.size(); i++)
					{
						bool isSelected = type_ids[type] == type_ids[i];

						if (ImGui::Selectable(type_name_str[i].data(), isSelected))
						{
							type = static_cast<LightType>(i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				//ImGuiLibrary::drawDragFloat3("type", type);
				Vector3f color3f = color;
				ImGuiLibrary::drawDragFloat3("color", color3f);
				color.r = color3f.r;
				color.g = color3f.g;
				color.b = color3f.b;
				ImGuiLibrary::drawDragFloat("Intensity", color.a);

				if (type == LightType::Point)
				{
					ImGuiLibrary::drawDragFloat("Range", params.x);
				}
				else if (type == LightType::Spot)
				{
					ImGuiLibrary::drawDragFloat("Range", params.x);

					float inner = MyMath::degrees(MyMath::acos(params.z));
					ImGuiLibrary::drawDragFloat("Inner Angle", inner);
					params.z = MyMath::cos(MyMath::radians(inner));

					float outer = MyMath::degrees(MyMath::acos(params.w));
					ImGuiLibrary::drawDragFloat("Outer Angle", outer);
					params.w = MyMath::cos(MyMath::radians(outer));
				}

				id--;

				ImGui::PopID();
				//ImGui::NewLine();
			});

		drawComponent<RigidbodyComponent>("Rigidbody", entity_,
			[](Entity entity_, auto& component_)
			{
				RigidbodyComponent& rigidbody = component_;

				{	// Body Type Combo
					std::array<const char*, 3>  bodyTypeStrings = { "Static", "Dynamic", "Kinematic" };
					const char* currentBodyTypeString = bodyTypeStrings[(int)rigidbody.type];
					if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
					{
						for (int i = 0; i < bodyTypeStrings.size(); i++)
						{
							bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
							if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
							{
								currentBodyTypeString = bodyTypeStrings[i];
								rigidbody.type = (RigidbodyComponent::BodyType)i;
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					ImGuiLibrary::drawDragFloat3("Velocity", rigidbody.velocity);
				}

				if (ImGui::TreeNodeEx("Constraints"))
				{
					ImGuiLibrary::drawVec3bCheckbox("Freeze Posiiton", rigidbody.isFreezePosiiton, 150.0f);
					ImGuiLibrary::drawVec3bCheckbox("Freeze Rotation", rigidbody.isFreezeRotation, 150.0f);
					ImGui::TreePop();
				}

				// Physics Body data
				if (ImGui::TreeNodeEx("Info"))
				{
					Body* pBody = static_cast<Body*>(rigidbody.pRuntimeBody);

					struct PhysicsData
					{
						PhysicsData() = default;
						PhysicsData(float speed_, const Vector3f& force_, const Vector3f& velocity_, const Vector3f& angularVelocity_, const Vector3f& inertiaTensor_,
							const Vector3f& inertiaTensorRotation_, const Vector3f& localCenterOfMass_, const Vector3f& worldCenterOfMass_)
							:
							speed(speed_), force(force_), velocity(velocity_), angularVelocity(angularVelocity_), inertiaTensor(inertiaTensor_),
							inertiaTensorRotation(inertiaTensorRotation_), localCenterOfMass(localCenterOfMass_), worldCenterOfMass(worldCenterOfMass_)
						{}
						void set(float speed_, const Vector3f& force_, const Vector3f& velocity_, const Vector3f& angularVelocity_, const Vector3f& inertiaTensor_,
							const Vector3f& inertiaTensorRotation_, const Vector3f& localCenterOfMass_, const Vector3f& worldCenterOfMass_)
						{
							speed = speed_;
							force = force_;  velocity = velocity_; angularVelocity = angularVelocity_;
							inertiaTensor = inertiaTensor_; inertiaTensorRotation = inertiaTensorRotation_;
							localCenterOfMass = localCenterOfMass_; worldCenterOfMass = worldCenterOfMass_;
						}

						float speed = 0.0f;
						Vector3f force = { 0.0f, 0.0f, 0.0f }; Vector3f velocity = { 0.0f, 0.0f, 0.0f }; Vector3f angularVelocity = { 0.0f, 0.0f, 0.0f };
						Vector3f inertiaTensor = { 0.0f, 0.0f, 0.0f }; Vector3f inertiaTensorRotation = { 0.0f, 0.0f, 0.0f };
						Vector3f localCenterOfMass = { 0.0f, 0.0f, 0.0f }; Vector3f worldCenterOfMass = { 0.0f, 0.0f, 0.0f };
					};

					// TODO: recitfy
					PhysicsData physicsData;
					if (pBody)
					{
						auto& data = pBody->m_data;
						physicsData.set(data.speed, data.debug_force, data.velocity, data.angularVelocity, data.angularVelocity,
							data.angularVelocity, data.angularVelocity, data.angularVelocity);
					}

					float indentationRatio = 0.2f;

					ImGuiLibrary::drawTextWithValue("Speed", physicsData.speed);
					ImGuiLibrary::drawTextWithValue("Force", physicsData.force, indentationRatio);
					ImGuiLibrary::drawTextWithValue("Velocity", physicsData.velocity, indentationRatio);
					ImGuiLibrary::drawTextWithValue("Angular Velocity", physicsData.angularVelocity, indentationRatio);
					ImGuiLibrary::drawTextWithValue("Inertia Tensor", physicsData.angularVelocity, indentationRatio);
					ImGuiLibrary::drawTextWithValue("Inertia Tensor Rotation", physicsData.angularVelocity, indentationRatio);
					ImGuiLibrary::drawTextWithValue("Local Center of Mass", physicsData.angularVelocity, indentationRatio);
					ImGuiLibrary::drawTextWithValue("World Center of Mass", physicsData.angularVelocity, indentationRatio);

					ImGui::TreePop();
				}

				//ImGui::NewLine();
			});

		drawComponent<NativeScriptComponent>("NativeScript", entity_,					// TODO: temp
			[](Entity entity_, auto& component_)
			{
				//ImGui::NewLine();
			});
#pragma region draw_collider
		drawComponent<CapsuleColliderComponent>("Capsule Collider", entity_,
			[](Entity entity_, CapsuleColliderComponent& component_)
			{
				auto& capsuleCollider = component_;
				ImGuiLibrary::drawCheckbox("is Trigger", capsuleCollider.isTrigger);
				ImGuiLibrary::drawDragFloat3("center", capsuleCollider.center);
				ImGuiLibrary::drawDragFloat("height", capsuleCollider.height);
				ImGuiLibrary::drawDragFloat("radius", capsuleCollider.radius);

				//ImGui::NewLine();
			});

		drawComponent<SphereColliderComponent>("Sphere Collider", entity_,
			[](Entity entity_, SphereColliderComponent& component_)
			{
				auto& sphereCollider = component_;
				ImGuiLibrary::drawCheckbox("is Trigger", sphereCollider.isTrigger);

				ImGuiLibrary::drawDragFloat3("center", sphereCollider.center);
				ImGuiLibrary::drawDragFloat("radius", sphereCollider.radius);

				//ImGui::NewLine();
			});

		drawComponent<PlaneColliderComponent>("Plane Collider", entity_,
			[](Entity entity_, PlaneColliderComponent& component_)
			{
				auto& planeCollider = component_;

				ImGuiLibrary::drawCheckbox("is Trigger", planeCollider.isTrigger);

				/*Vector3f normal = planeCollider.normal;
				if (ImGuiLibrary::drawDragFloat3("normal", normal))
				{
					planeCollider.normal = MyMath::normalize(normal);
				}*/

				//ImGui::NewLine();
			});

		drawComponent<AABBColliderComponent>("AABB Collider", entity_,				// TODO: remove temp
			[](Entity entity_, AABBColliderComponent& component_)
			{
				auto& aabbCollider = component_;

				ImGuiLibrary::drawCheckbox("is Trigger", aabbCollider.isTrigger);

				ImGuiLibrary::drawDragFloat3("center", aabbCollider.center);
				ImGuiLibrary::drawDragFloat3("size", aabbCollider.size);

				//ImGui::NewLine();
			});

		drawComponent<OBBColliderComponent>("OBB Collider", entity_,				// TODO: remove temp
			[](Entity entity_, OBBColliderComponent& component_)
			{
				auto& obbCollider = component_;

				ImGuiLibrary::drawCheckbox("is Trigger", obbCollider.isTrigger);

				ImGuiLibrary::drawDragFloat3("center", obbCollider.center);
				ImGuiLibrary::drawDragFloat3("size", obbCollider.size);

				//ImGui::NewLine();
			});
#pragma endregion

		drawComponent<SpriteRendererComponent>("Sprite Renderer", entity_,			// TODO: recitfy
			[](Entity entity_, SpriteRendererComponent& component_)
			{
				auto& spriteRenderer = component_;
				//spriteRenderer.;

				//ImGui::NewLine();
			});

		drawComponent<BoneTransformComponent>("BoneTransform", entity_,			// TODO: remove
			[](Entity entity_, auto& component_)
			{
				auto& boneTransform = *component_.pBoneTransform;

				ImGuiLibrary::drawVec3fControl("b_position", boneTransform.translation);
				Vector3f rotation = MyMath::degrees(boneTransform.rotation);
				ImGuiLibrary::drawVec3fControl("b_rotation", rotation);
				boneTransform.rotation = MyMath::radians(rotation);
				ImGuiLibrary::drawVec3fControl("b_scale", boneTransform.scale, 1.0f);

				ImGuiLibrary::drawVec3fControl("ex_position", boneTransform.ex_translation);
				Vector3f ex_rotation = MyMath::degrees(boneTransform.ex_rotation);
				ImGuiLibrary::drawVec3fControl("ex_rotation", ex_rotation);
				boneTransform.ex_rotation = MyMath::radians(ex_rotation);
				ImGuiLibrary::drawVec3fControl("ex_scale", boneTransform.ex_scale, 1.0f);

				//ImGui::NewLine();
			});

		drawComponent<AnimationComponent>("Animation", entity_,					// TODO: temp
			[](Entity entity_, auto& component_)
			{
				//ImGui::NewLine();
			});

		drawComponent<ParticleSystemComponent>("Particle System", entity_,		// TODO: temp
			[](Entity entity_, auto& component_)
			{
				ImGuiLibrary::drawInputInt("count", component_.particlesCount);

				//ImGui::NewLine();
			});
	}

#pragma region create_entities

	Entity ScenePanel::createLight(Entity parentEntity_, const std::string& name_)
	{
		Entity entity;
		if (parentEntity_)
			entity = m_pScene->createEntity(name_, parentEntity_);
		else
			entity = m_pScene->createEntity(name_);

		entity.addComponent<LightComponent>();
		return entity;
	}

	template<typename ColliderType>
	static Entity createPrimitiveMesh(PrimitiveMesh primitiveMesh_, Entity parentEntity_, const std::string& name_, RefPtr<Scene>& pScene_)
	{
		Entity entity;
		if (parentEntity_)
			entity = pScene_->createEntity(name_, parentEntity_);
		else
			entity = pScene_->createEntity(name_);

		entity.addComponent<ColliderType>();

		auto& entity_mesh = entity.addComponent<MeshComponent>();
		entity_mesh.set(AssetManager::getMesh(primitiveMesh_));
		entity.addComponent<MeshRendererComponent>(AssetManager::getRendererShader());		// TODO: recitfy temp
		auto& entity_body = entity.addComponent<RigidbodyComponent>();
		entity_body.type = RigidbodyComponent::BodyType::Dynamic;

		return entity;
	}

	Entity ScenePanel::createAABB(Entity parentEntity_, const std::string& name_)
	{
		return createPrimitiveMesh<AABBColliderComponent>(PrimitiveMesh::Box, parentEntity_, name_, m_pScene);
	}
	Entity ScenePanel::createCapsule(Entity parentEntity_, const std::string& name_)
	{
		return createPrimitiveMesh<CapsuleColliderComponent>(PrimitiveMesh::Capsule, parentEntity_, name_, m_pScene);
	}
	Entity ScenePanel::createOBB(Entity parentEntity_, const std::string& name_)
	{
		return createPrimitiveMesh<OBBColliderComponent>(PrimitiveMesh::Box, parentEntity_, name_, m_pScene);
	}
	Entity ScenePanel::createSphere(Entity parentEntity_, const std::string& name_)
	{
		return createPrimitiveMesh<SphereColliderComponent>(PrimitiveMesh::Sphere, parentEntity_, name_, m_pScene);
	}
	Entity ScenePanel::createPlane(Entity parentEntity_, const std::string& name_)
	{
		return createPrimitiveMesh<PlaneColliderComponent>(PrimitiveMesh::Plane, parentEntity_, name_, m_pScene);
	}

	Entity ScenePanel::createTriangle(Entity parentEntity_, const std::string& name_)
	{
		Entity entity;
		if (parentEntity_)
			entity = m_pScene->createEntity(name_, parentEntity_);
		else
			entity = m_pScene->createEntity(name_);

		entity.addComponent<TriangleColliderComponent>();

		auto& entity_body = entity.addComponent<RigidbodyComponent>();
		entity_body.type = RigidbodyComponent::BodyType::Static;

		return entity;
	}
#pragma endregion
}