#include "MocapLayer.h"

#include "Control/VirtualMouse.h"



namespace vog
{
	MocapLayer::MocapLayer(const std::string& name_)
		:
		Layer(name_), m_cameraController()
	{
		m_pActiveScene = createRefPtr<Scene>();
		m_pEditorScene = m_pActiveScene;

#ifdef VOG_ENABLE_KINECT
		m_pKinectAPI = createScopePtr<KinectAPI>();
#endif // VOG_ENABLE_KINECT

		m_arduinoSerial.init();

		m_trail_left.init();
		m_trail_right.init();
	}

	MocapLayer::~MocapLayer()
	{
		m_trail_left.destroy();
		m_trail_right.destroy();
	}

	static Entity createRawCube(const std::string& name_, RefPtr<Scene>& pScene_)
	{
		Entity entity;
		entity = pScene_->createEntity(name_);
		auto& entity_mesh = entity.addComponent<MeshComponent>();
		entity_mesh.set(AssetManager::getMesh(PrimitiveMesh::Box));
		entity.addComponent<MeshRendererComponent>(AssetManager::getRendererShader());		// TODO: recitfy temp
		//auto& entity_body = entity.addComponent<RigidbodyComponent>();
		//entity_body.type = RigidbodyComponent::BodyType::Static;
		return entity;
	}

	void MocapLayer::onAttach()
	{


#if 1
		// Kinect test
		Entity modelEntity = m_pActiveScene->loadModel("assets/meshes/res/model.fbx");
		if (modelEntity.hasComponent<AnimationComponent>())
		{
			auto& mesh = modelEntity.getComponent<AnimationComponent>();
			m_pKinectModel = mesh.pModel;
		}
		m_modelHandle = modelEntity;
#endif // 0

		m_scenePanel.setScene(m_pActiveScene);

		// lighting setup
		{
			auto light = m_scenePanel.createLight();
			light.getComponent<LightComponent::Type>().type = LightType::Directional;
			light.getComponent<TransformComponent>().translation = { 1.0f, 5.0f, 0.0f };
			light.getComponent<TransformComponent>().rotation = { MyMath::radians(150.0f), 0.0f, 0.0f };
		}
		auto pRhythm_emission_map = Texture2D::create("assets/textures/demo_game/hit_rhythm_emission_map.png");

		// setup object in scene
		{
			m_pRedTexture = Texture2D::create(1, 1);
			uint32_t data = 0xff1111ff;
			m_pRedTexture->setData(&data, sizeof(uint32_t));

			m_pBlueTexture = Texture2D::create(1, 1);
			data = 0x0099ffff;
			m_pBlueTexture->setData(&data, sizeof(uint32_t));

			m_pWhiteTexture = Texture2D::create(1, 1);
			data = 0xffffffff;
			m_pWhiteTexture->setData(&data, sizeof(uint32_t));

			//set up saber
			{
				auto saber_left_parent = m_pActiveScene->createEntity("saber_left");
				m_leftSaberHandle = saber_left_parent;
				auto* pNSC = &saber_left_parent.addComponent<NativeScriptComponent>();
				pNSC->bind<demo_game::Saber>();
				pNSC->pUeserData = &m_isSaberAnimate;
				saber_left_parent.addComponent<RigidbodyComponent>();
				auto& saber_left_capsule = saber_left_parent.addComponent<CapsuleColliderComponent>();
				saber_left_capsule.center = { 0.0f, 6.0f, -0.075f };
				saber_left_capsule.height = 11.5f;
				saber_left_capsule.radius = 0.25f;

				auto saber_left = m_pActiveScene->loadModel("assets/meshes/light_saber/light_saber.obj", saber_left_parent);
				saber_left.getComponent<TransformComponent>().rotation = Vector3f(0.0f, MyMath::radians(180.0f), 0.0f);
				auto saber_left_child_handle = saber_left.getComponent<ecs::HierarchyComponent>().children[0];
				auto saber_left_child = Entity{ saber_left_child_handle, m_pActiveScene.get() };
				saber_left_child.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pRedTexture);
				saber_left_child.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::red_saber_albedo_color);
				saber_left_child.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", demo_game::ColorType::red_saber_emissive_color);

				// Left trail
				{
					Entity entity = m_scenePanel.createSphere({}, "Left Trail");
					auto* pNsc = &entity.addComponent<NativeScriptComponent>();
					pNsc->bind<demo_game::SaberTrail>();

					m_trail_left.width = 11.45f;
					m_trail_left.lifeTime = 0.3f;
					m_trail_left.offset.y = 0.22f;

					m_saberTrailData_left.pTrail = &m_trail_left;
					m_saberTrailData_left.target = m_leftSaberHandle;

					pNsc->pUeserData = &m_saberTrailData_left;

					auto& mesh = entity.getComponent<MeshComponent>();
					mesh.pVertexBuffer = m_trail_left.pVertexBuffer;
					mesh.pIndexBuffer = m_trail_left.pIndexBuffer;

					auto& meshRenderer = entity.getComponent<MeshRendererComponent>();
					meshRenderer.pMaterial = Material::create(m_trail_left.pShader);
					meshRenderer.pMaterial->setTexture2D("u_texture", m_pRedTexture);
					auto emmision = demo_game::ColorType::red_saber_emissive_color;
					emmision.w = 0.1f;
					meshRenderer.pMaterial->setFloat4("u_emission", emmision);
				}

				auto saber_right_parent = m_pActiveScene->createEntity("saber_right");
				m_rightSaberHandle = saber_right_parent;
				auto saber_right = m_pActiveScene->duplicateEntity(saber_left, saber_right_parent);
				saber_right.getComponent<TransformComponent>().rotation = Vector3f(0.0f, MyMath::radians(180.0f), 0.0f);
				pNSC = &saber_right_parent.addComponent<NativeScriptComponent>();
				pNSC->bind<demo_game::Saber>();
				pNSC->pUeserData = &m_isSaberAnimate;
				saber_right_parent.addComponent<RigidbodyComponent>();
				auto& saber_right_capsule = saber_right_parent.addComponent<CapsuleColliderComponent>();
				saber_right_capsule.center = { 0.0f, 6.0f, -0.075f };
				saber_right_capsule.height = 11.5f;
				saber_right_capsule.radius = 0.25f;

				auto saber_right_child_handle = saber_right.getComponent<ecs::HierarchyComponent>().children[0];
				auto saber_right_child = Entity{ saber_right_child_handle, m_pActiveScene.get() };
				auto& pOld_Material = saber_right_child.getComponent<MeshRendererComponent>().pMaterial;
				saber_right_child.getComponent<MeshRendererComponent>().pMaterial = Material::create(pOld_Material);
				saber_right_child.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pBlueTexture);
				saber_right_child.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::blue_saber_albedo_color);
				saber_right_child.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", demo_game::ColorType::blue_saber_emissive_color);

				// Right trail
				{
					Entity entity = m_scenePanel.createSphere({}, "Right Trail");
					auto* pNsc = &entity.addComponent<NativeScriptComponent>();
					pNsc->bind<demo_game::SaberTrail>();

					m_trail_right.width = 11.45f;
					m_trail_right.lifeTime = 0.3f;
					m_trail_right.offset.y = 0.22f;

					m_saberTrailData_right.pTrail = &m_trail_right;
					m_saberTrailData_right.target = m_rightSaberHandle;

					pNsc->pUeserData = &m_saberTrailData_right;

					auto& mesh = entity.getComponent<MeshComponent>();
					mesh.pVertexBuffer = m_trail_right.pVertexBuffer;
					mesh.pIndexBuffer = m_trail_right.pIndexBuffer;

					auto& meshRenderer = entity.getComponent<MeshRendererComponent>();
					meshRenderer.pMaterial = Material::create(m_trail_right.pShader);
					meshRenderer.pMaterial->setTexture2D("u_texture", m_pBlueTexture);
					auto emmision = demo_game::ColorType::blue_saber_emissive_color;
					emmision.w = 0.1f;
					meshRenderer.pMaterial->setFloat4("u_emission", emmision);
				}
			}

			// set up scene
			{
				Entity entity = createRawCube("Player Stage", m_pActiveScene);
				auto* pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { 0.0f, -5.0f, 0.0f };
				pTransform->scale = {30.0f, 0.5f, 15.0f};
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", { 0.05f, 0.05f, 0.05f, 1.0f });

				entity = createRawCube("Stage", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { 0.0f, -5.0f, -263.5f };
				pTransform->scale = { 50.0f, 0.5f, 500.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", { 0.2f, 0.2f, 0.2f, 1.0f });

				entity = createRawCube("Left Wall", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = {-14.0f, 25.0f, 0.0f};
				pTransform->scale = { 1.0f, 100.0f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", { 0.1f, 0.1f, 0.1f, 1.0f });

				entity = createRawCube("Right Wall", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { 14.0f, 25.0f, 0.0f };
				pTransform->scale = { 1.0f, 100.0f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", { 0.1f, 0.1f, 0.1f, 1.0f });

				entity = createRawCube("Upper Wall", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation.y = +22.0f;
				pTransform->scale = { 400.0f, 0.5f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", { 0.05f, 0.05f, 0.05f, 1.0f });

				entity = createRawCube("Right Bar0", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { 13.4f, 6.0f, 0.0f };
				pTransform->scale = { 0.25f, 0.25f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pRedTexture);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::red_bar_albedo_color);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", demo_game::ColorType::red_bar_emissive_color);

				entity = createRawCube("Right Bar1", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { 13.4f, 10.0f, 0.0f };
				pTransform->scale = { 0.25f, 0.25f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pRedTexture);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::red_bar_albedo_color);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", demo_game::ColorType::red_bar_emissive_color);

				entity = createRawCube("Left Bar0", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { -13.4f, 6.0f, 0.0f };
				pTransform->scale = { 0.25f, 0.25f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pBlueTexture);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::blue_bar_albedo_color);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", demo_game::ColorType::blue_bar_emissive_color);

				entity = createRawCube("Left Bar1", m_pActiveScene);
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation = { -13.4f, 10.0f, 0.0f };
				pTransform->scale = { 0.25f, 0.25f, 1000.0f };
				entity.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pBlueTexture);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::blue_bar_albedo_color);
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", demo_game::ColorType::blue_bar_emissive_color);

				entity = m_scenePanel.createPlane({}, "Boundary");
				entity.removeComponent<MeshRendererComponent>();
				pTransform = &entity.getComponent<TransformComponent>();
				pTransform->translation.z = 13.0f;
				pTransform->rotation.x = MyMath::radians(90.0f);
				pTransform->scale = {50.0f, 1.0f, 50.0f};
				entity.addComponent<NativeScriptComponent>().bind<demo_game::Boundary>();
			}

			{
				Entity lightEntity = m_scenePanel.createLight({}, "Light-Spoit");
				auto* pType = &lightEntity.getComponent<LightComponent::Type>();
				pType->type = LightType::Spot;

				auto* pLight_Colors = &lightEntity.getComponent<LightComponent::Color>().color;
				auto* pLight_Params = &lightEntity.getComponent<LightComponent::Params>().params;
				*pLight_Colors = Vector4f(MyRandom::Vec3f(1.0f, 5.0f), 1.0f);
				pLight_Colors->a = MyRandom::Float(2.0f, 4.0f);
				pLight_Params->x = MyRandom::Float(5.0f, 10.0f);

				pLight_Params->z = 10.0f;
				pLight_Params->w = 30.0f;

			}

			// set up hit light
			{
				auto light = m_scenePanel.createLight();
				light.getComponent<LightComponent::Type>().type = LightType::Directional;
				auto* pColors = &light.getComponent<LightComponent::Color>().color;
				auto* pParams = &light.getComponent<LightComponent::Params>().params;

				pColors->a = 0.1f;
				light.getComponent<TransformComponent>().translation = { 1.0f, 5.0f, 0.0f };
				light.getComponent<TransformComponent>().rotation = { MyMath::radians(150.0f), 0.0f, 0.0f };

				auto& light_nsc = light.addComponent<NativeScriptComponent>();
				light_nsc.bind<demo_game::LightManager>();
				light_nsc.pUeserData = (void*)&m_lightHandles;
				m_lightHandles.reserve(demo_game::LightManager::s_max_count + 1);

				auto light_mesh = m_scenePanel.createPlane({}, "light plane");
				auto* pTransform = &light_mesh.getComponent<TransformComponent>();
				pTransform->translation.z = -200.0f;
				pTransform->rotation = { MyMath::radians(90.0f), 0.0f, 0.0f};
				pTransform->scale = Vector3f(20.0f, 20.0f, 20.0f);
				light_mesh.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::blue_plane_albedo_color);
				//light_mesh.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emissive", demo_game::ColorType::blue_bar_emissive_color);
				m_lightHandles.push_back(light_mesh);			// [0]

				Entity lightEntity = m_scenePanel.createLight({}, "light plane light");
				pTransform = &lightEntity.getComponent<TransformComponent>();
				pTransform->translation = light_mesh.getComponent<TransformComponent>().translation;
				pTransform->translation.y = 10.0f;
				pTransform->translation.z += 5.0f;
				auto* pType = &light.getComponent<LightComponent::Type>();

				pColors = &lightEntity.getComponent<LightComponent::Color>().color;
				*pColors = Vector4f(MyRandom::Vec3f(1.0f, 2.0f), 1.0f);
				pColors->a = 2.0f;
				pParams = &lightEntity.getComponent<LightComponent::Params>().params;
				pParams->x = 200.0f;

				m_lightHandles.push_back(lightEntity);			// [1]

				for (size_t i = 1; i < demo_game::LightManager::s_max_count; i++)
				{
					Entity lightEntity = m_scenePanel.createLight({}, "Light");
					auto* pType = &lightEntity.getComponent<LightComponent::Type>();
					pType->type = LightType::Spot;

					auto* pLight_Colors = &lightEntity.getComponent<LightComponent::Color>().color;
					auto* pLight_Params = &lightEntity.getComponent<LightComponent::Params>().params;
					*pLight_Colors = Vector4f(MyRandom::Vec3f(1.0f, 5.0f), 1.0f);
					pLight_Colors->a = MyRandom::Float(90.0f, 120.0f);
					pLight_Params->x = MyRandom::Float(5.0f, 10.0f);
					float inner_angle = 10.0f;
					pLight_Params->z = MyMath::cos(MyMath::radians(inner_angle));
					pLight_Params->w = MyMath::cos(MyMath::radians(MyRandom::Float(inner_angle + 10.0f, inner_angle + 40.0f)));

					auto& transform = lightEntity.getComponent<TransformComponent>();
					transform.translation.x = MyRandom::Float(-10.0f, 10.0f);
					transform.translation.y = MyRandom::Float(1.0f, 5.0f);

					//transform.isEnable = false;
					transform.translation.z -= i * 5.0f - 5.0f;

					transform.rotation.x = MyMath::radians(90.0f);

					m_lightHandles.push_back(lightEntity);
				}
			}

			// set up rhythmManager
			{
				auto rhythmCubeManager = m_pActiveScene->createEntity("rhythmCubeMangaer");
				auto& rhythmCubeManagerTransform = rhythmCubeManager.getComponent<TransformComponent>();
				rhythmCubeManagerTransform.translation = { 0.0f, 0.0f, -45.0f };
				auto& nsc = rhythmCubeManager.addComponent<NativeScriptComponent>();
				nsc.bind<demo_game::RhythmCubeManager>();
				rhythmCubeManager.addComponent<LightComponent>();
				nsc.pUeserData = &m_rhythmHandles;
				m_rhythmHandles.reserve(demo_game::RhythmCubeManager::s_max_rhythmCube);

				for (size_t i = 0; i < demo_game::RhythmCubeManager::s_max_rhythmCube; i++)
				{
					auto rhythmCube = m_scenePanel.createOBB({}, "RhythmCube");
					auto& cube_nsc = rhythmCube.addComponent<NativeScriptComponent>();
					cube_nsc.bind<demo_game::RhythmCube>();

					auto& transform = rhythmCube.getComponent<TransformComponent>();
					transform.isEnable = false;
					transform.translation = rhythmCubeManagerTransform.translation;
					transform.scale = { 2.0f, 2.0f, 2.0f };
					m_rhythmHandles.push_back(rhythmCube);
				}
			}
		}
	}

	void MocapLayer::onDetach()
	{
	}

	void MocapLayer::onUpdate(float timestep_)
	{
		updateMocap(timestep_);

		// Resize
		if (auto viewportSize = DeferredRenderer::getViewportSize();
			m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(viewportSize.first != m_viewportSize.x || viewportSize.second != m_viewportSize.y))
		{
			uint32_t width = static_cast<uint32_t>(m_viewportSize.x);
			uint32_t height = static_cast<uint32_t>(m_viewportSize.y);

			DeferredRenderer::onWindowResize(width, height);
			Renderer::onWindowResize(width, height);
			RendererDebug::onWindowResize(width, height);

#ifdef VOG_ENABLE_KINECT
			m_pKinectAPI->onResize(m_viewportSize.x, m_viewportSize.y);
#endif // VOG_ENABLE_KINECT

			m_cameraController.setAspect(m_viewportSize.x / m_viewportSize.y);
		}

#ifdef VOG_ENABLE_KINECT
		/*if (m_pKinectAPI->getIsHandClosed().first)
		{
			VirtualMouse mouse;
			auto handPosition = m_pKinectAPI->getHandScreenPosition();
			Vector2f ratio = { handPosition.first.x, handPosition.first.y };
			mouse.setMousePosition((int)ratio.x, (int)ratio.y);
			VOG_LOG_INFO("X: {0}, Y: {1}", ratio.x, ratio.y);
		}*/
#endif // VOG_ENABLE_KINECT
		
		if (m_isViewportFocused)
			m_cameraController.onUpdate(timestep_);

		switch (m_sceneState)
		{
			case EditorSceneState::Edit:
			{
				m_pActiveScene->onUpdateEditor(timestep_, m_cameraController.getCamera());
			}break;
			case EditorSceneState::Play:
			{
				m_pActiveScene->onUpdateRuntime(timestep_, m_cameraController.getCamera());
			}break;
		}

		{	// Render Debug stuff
			onDebugRender();
		}

		AssetManager::endScene();
	}

	void MocapLayer::onEvent(Event& event_)
	{
		EventDispatcher dispatcher(event_);

		//VOG_LOG_INFO(m_name + "onEvent()!");
		m_cameraController.onEvent(event_);

		dispatcher.dispatch<MouseButtonPressedEvent>(std::bind(&MocapLayer::onMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.dispatch<KeyPressedEvent>(std::bind(&MocapLayer::onKeyPressed, this, std::placeholders::_1));
	}

	void MocapLayer::onImGuiRender()
	{
		//return;

#pragma region DockSpace
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) Application::get().close();
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
#pragma endregion

#ifdef VOG_ENABLE_KINECT
		m_pKinectAPI->onImGuiRender();
#endif // VOG_ENABLE_KINECT

		ImGui::Begin("IMU");

		m_arduinoSerial.onImGuiRender();

		ImGui::End();

#pragma region Viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin("Viewport");

		m_isViewportFocused = ImGui::IsWindowFocused();
		m_isViewportHovered = ImGui::IsWindowHovered();

		Application::get().getImGuiLayer().setBlockEvents(!m_isViewportFocused || !m_isViewportHovered);

		//VOG_LOG_WARN("m_isViewportFocused: {0}", m_isViewportFocused);
		//VOG_LOG_WARN("m_isViewportHovered: {0}", m_isViewportHovered);

		ImVec2 viewportPlaneSize = ImGui::GetContentRegionAvail();
		m_viewportSize = { viewportPlaneSize.x , viewportPlaneSize.y };

		if (Input::isMousePressed(KeyCode::LButton))
		{
			Vector2f mouse = getViewportMousePosition();

			if (mouse.x >= 0 && mouse.y >= 0 && mouse.x < (int)m_viewportSize.x && mouse.y < (int)m_viewportSize.y)
			{
				mouse = mouse / m_viewportSize;
				// TO NDC space
				mouse = 2.0f * mouse - 1.0f;
				mouse.y = -mouse.y;

				//VOG_LOG_INFO("mouse X: {0}, mouse Y: {1}", mouse.x, mouse.y);

				Vector4f a = Vector4f(mouse.x, mouse.y, 0.0f, 1.0f);
				Vector4f b = Vector4f(mouse.x, mouse.y, 1.0f, 1.0f);

				a = m_cameraController.getCamera().getInverseViewProjectionMatrix() * a;
				b = m_cameraController.getCamera().getInverseViewProjectionMatrix() * b;

				a /= a.w;
				b /= b.w;

				m_ray.origin = a;
				m_ray.direction = MyMath::normalize(b - a);
				//m_pActiveScene->raycast(m_ray.origin, m_ray.direction);
			}
		}

		static int index = 0;		// TODO: remove

		//ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		//ImGui::Image(reinterpret_cast<void*>((uint64_t)DeferredRenderer::getReusltID(0)), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::Image(reinterpret_cast<void*>((uint64_t)RendererDebug::getReusltID(0)), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		//ImGui::GetForegroundDrawList()->AddText(ImGui::GetIO().FontDefault, 48.0f, {50.0f, 50.0f}, 0xffffffff, "Hello");

		//VOG_CORE_LOG_INFO("Framebuffer Texture id: {0}", DeferredRenderer::getReusltID(0));

		ImGui::PopStyleVar();

		ImGui::End();
#pragma endregion


#pragma region Settings
		ImGui::Begin("Settings");
		//ImGui::ColorEdit3("Square Color", a);

		ImGuiLibrary::drawDragFloat("Camera Speed", m_cameraController.getSpeed());
		ImGuiLibrary::drawDragFloat("Camera Rotation Speed", m_cameraController.getRotationtSpeed());
		
		ImGuiLibrary::drawCheckbox("Enable Collider Outline", m_isEnableColliderOutline);
		ImGuiLibrary::drawCheckbox("Enable Light Outline", m_isEnableLightOutline);
		ImGuiLibrary::drawCheckbox("Enable Saber Animate", m_isSaberAnimate);

		m_trail_left.onImGuiRender();
		

		// Scene State
		std::string state_str;
		EditorSceneState sceneState = m_sceneState;

		if (m_sceneState == EditorSceneState::Edit)
		{
			state_str = "Current Mode: Edit";
		}
		else if (m_sceneState == EditorSceneState::Play)
		{
			state_str = "Current Mode: Play";
		}
		if (ImGuiLibrary::drawCheckbox(state_str, (bool&)sceneState))
		{
			if (m_sceneState == EditorSceneState::Edit)
			{
				onScenePlay();
			}
			else if (m_sceneState == EditorSceneState::Play)
			{
				onSceneStop();
			}
		}
		ImGui::End();

		//m_pKinectAPI->onImGuiRender();
#pragma endregion

		m_scenePanel.onImGuiRender();

		{
			ImGui::Begin("Renderer Material");

			DeferredRenderer::onImGuiRender();
			RendererDebug::onImGuiRender();

			ImGui::End();
		}

#if 0
		{
			ImGui::Begin("Depth");

			//ImGui::Image(reinterpret_cast<void*>((uint64_t)DeferredRenderer::s_pData->geometryPass.getFramebuffer()->getDepthAttachmentID()), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			//ImGuiLibrary::drawTextureIcon("Depth", DeferredRenderer::s_pData->geometryPass.getFramebuffer()->getColorAttachmentID(), { 1280, 720 });
			ImGui::Image(reinterpret_cast<void*>((uint64_t)DeferredRenderer::temp_getShadowmap()), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			ImGui::End();
		}
#endif // 0
		

		ImGui::End();



		ImGuiLibrary::showDebugMetrics();
		ImGuiLibrary::showImGuiSetting();


		//static bool op = true;
		//ImGui::ShowDemoWindow(&op);
	}

	void MocapLayer::onScenePlay()
	{
		m_pActiveScene = Scene::copy(m_pEditorScene);
		m_pActiveScene->onRuntimeStart();
		m_scenePanel.setScene(m_pActiveScene);

		m_sceneState = EditorSceneState::Play;
	}

	void MocapLayer::onSceneStop()
	{
		m_pActiveScene->onRuntimeStop();
		m_pActiveScene = m_pEditorScene;
		m_scenePanel.setScene(m_pActiveScene);

		m_sceneState = EditorSceneState::Edit;
	}

	void MocapLayer::renderColliderOutline()
	{
		auto& transform_cm = m_pActiveScene->getComponentManager<TransformComponent>();

		auto& capsuleCollider_cm = m_pActiveScene->getComponentManager<CapsuleColliderComponent>();
		auto& sphereCollider_cm = m_pActiveScene->getComponentManager<SphereColliderComponent>();
		auto& planeCollider_cm = m_pActiveScene->getComponentManager<PlaneColliderComponent>();
		auto& aabbCollider_cm = m_pActiveScene->getComponentManager<AABBColliderComponent>();
		auto& obbCollider_cm = m_pActiveScene->getComponentManager<OBBColliderComponent>();
		auto& triangleCollider_cm = m_pActiveScene->getComponentManager<TriangleColliderComponent>();

		auto& capsuleColliderComponents = capsuleCollider_cm.getComponents();
		auto& sphereColliderComponents = sphereCollider_cm.getComponents();
		auto& planeColliderComponents = planeCollider_cm.getComponents();
		auto& aabbColliderComponents = aabbCollider_cm.getComponents();
		auto& obbColliderComponents = obbCollider_cm.getComponents();
		auto& triangleColliderComponents = triangleCollider_cm.getComponents();

		auto color = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
		float thickness = 0.02f;

		float toModelScaleBaseRatio = 2.0f;
		//float toModelScaleRatioTerm = 0.05f;

		for (size_t i = 0; i < capsuleCollider_cm.getCount(); i++)
		{
			auto entityHandle = capsuleCollider_cm.getEntityHandle(i);
			auto& capsuleCollider = capsuleColliderComponents[i];
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			Vector3f translation;
			Vector3f scale;
			Vector3f rotation;

			MyMath::decompose(transform.worldMatrix, scale, rotation, translation);

			auto radius = MyMath::max(scale.x, scale.z) * (capsuleCollider.radius);
			auto normal = MyMath::eulerToMatrix3f(rotation) * Vector3f(0.0f, 1.0f, 0.0f);

			translation = translation + MyMath::toQuaternion(rotation) * capsuleCollider.center;
			auto half_height = capsuleCollider.height * 0.5f * scale.y;
			auto base_center = translation - normal * (half_height - radius);
			auto tip_center = translation + normal * (half_height - radius);

			scale = Vector3f(1.0f, 1.0f, 1.0f) * radius * (toModelScaleBaseRatio);		// model 2r = 2m
			//rotation = rotation;

			Matrix4f ret_base_transform = MyMath::translate(base_center) * MyMath::eulerToMatrix4f(rotation) * MyMath::scale(scale);
			Matrix4f ret_tip_transform = MyMath::translate(tip_center) * MyMath::eulerToMatrix4f(rotation) * MyMath::scale(scale);

			RendererDebug::drawDebugSphere(ret_base_transform, color, thickness);
			RendererDebug::drawDebugSphere(ret_tip_transform, color, thickness);
		}

		toModelScaleBaseRatio = 2.0f;
		//toModelScaleRatioTerm = 0.05f;

		for (size_t i = 0; i < sphereCollider_cm.getCount(); i++)
		{
			auto entityHandle = sphereCollider_cm.getEntityHandle(i);
			auto& sphereCollider = sphereColliderComponents[i];
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			Vector3f translation;
			Vector3f scale;
			Vector3f rotation;

			MyMath::decompose(transform.worldMatrix, scale, rotation, translation);

			translation = translation + sphereCollider.center;
			scale = Vector3f(1.0f, 1.0f, 1.0f) * MyMath::max(scale.x, MyMath::max(scale.y, scale.z))
				* sphereCollider.radius * (toModelScaleBaseRatio);		// model 2r = 2m

			Matrix4f ret_transform = MyMath::translate(translation) * MyMath::eulerToMatrix4f(rotation) * MyMath::scale(scale);
			RendererDebug::drawDebugSphere(ret_transform, color, thickness);

			//Vector3f translation = transform.translation + sphereCollider.center;
			//Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f) * MyMath::max(transform.scale.x, MyMath::max(transform.scale.y, transform.scale.z)) 
			//					* sphereCollider.radius * (toModelScaleBaseRatio);		// model 2r = 2m

			//Matrix4f ret_transform = MyMath::translate(translation) * MyMath::eulerToMatrix4f(transform.rotation) * MyMath::scale(scale);
			//RendererDebug::drawDebugSphere(ret_transform, color, thickness);
		}

		toModelScaleBaseRatio = 2.0f;
		//toModelScaleRatioTerm = 0.01f;
		for (size_t i = 0; i < planeCollider_cm.getCount(); i++)
		{
			auto entityHandle = planeCollider_cm.getEntityHandle(i);
			auto& planeCollider = planeColliderComponents[i];
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			Vector3f translation = transform.translation;
			Vector3f scale = transform.scale * (toModelScaleBaseRatio);		// model 2r = 2m

			Matrix4f ret_transform = MyMath::translate(translation) * MyMath::eulerToMatrix4f(transform.rotation) * MyMath::scale(scale);

			RendererDebug::drawPlane(ret_transform, color);
		}

		toModelScaleBaseRatio = 2.0f;
		//toModelScaleRatioTerm = 0.01f;
		for (size_t i = 0; i < aabbCollider_cm.getCount(); i++)
		{
			auto entityHandle = aabbCollider_cm.getEntityHandle(i);
			auto& aabbCollider = aabbColliderComponents[i];
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			Vector3f translation = transform.translation + aabbCollider.center;
			Vector3f scale = transform.scale * aabbCollider.size * (toModelScaleBaseRatio);		// model 2r = 2m
			// aabb dont have rotation
			Matrix4f ret_transform = MyMath::translate(translation) * MyMath::scale(scale);

			RendererDebug::drawLineBox(ret_transform, color);
		}

		toModelScaleBaseRatio = 2.0f;
		//toModelScaleRatioTerm = 0.01f;
		for (size_t i = 0; i < obbCollider_cm.getCount(); i++)
		{
			auto entityHandle = obbCollider_cm.getEntityHandle(i);
			auto& obbCollider = obbColliderComponents[i];
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			if (!transform.isEnable)
				continue;

			Vector3f translation = transform.translation + obbCollider.center;
			Vector3f scale = transform.scale * obbCollider.size * (toModelScaleBaseRatio);		// model 2r = 2m
			Vector3f rotation = transform.rotation;
			Matrix4f ret_transform = MyMath::translate(translation) * MyMath::eulerToMatrix4f(transform.rotation) * MyMath::scale(scale);

			RendererDebug::drawLineBox(ret_transform, color);
		}

		toModelScaleBaseRatio = 1.0f;
		//toModelScaleRatioTerm = 0.01f;
		for (size_t i = 0; i < triangleCollider_cm.getCount(); i++)
		{
			auto entityHandle = triangleCollider_cm.getEntityHandle(i);
			auto& triangleCollider = triangleColliderComponents[i];
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			Vector3f translation = transform.translation;
			Vector3f scale = transform.scale * (toModelScaleBaseRatio);		
			Vector3f rotation = transform.rotation;
			Matrix4f ret_transform = MyMath::translate(translation) * MyMath::eulerToMatrix4f(transform.rotation) * MyMath::scale(scale);

			RendererDebug::drawTriangle(ret_transform, color);
		}
	}

	void MocapLayer::renderLightOutline()
	{
		auto& transform_cm		= m_pActiveScene->getComponentManager<TransformComponent>();
		auto& light_cm			= m_pActiveScene->getComponentManager<LightComponent>();
		auto& light_type_cm		= m_pActiveScene->getComponentManager<LightComponent::Type>();
		auto& light_position_cm = m_pActiveScene->getComponentManager<LightComponent::Position>();
		auto& light_color_cm	= m_pActiveScene->getComponentManager<LightComponent::Color>();
		auto& light_params_cm	= m_pActiveScene->getComponentManager<LightComponent::Params>();

		VOG_CORE_ASSERT(light_cm.getCount() == light_position_cm.getCount()
			&& light_cm.getCount() == light_color_cm.getCount()
			&& light_cm.getCount() == light_params_cm.getCount(), "");

		auto color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
		float thickness = 0.02f;

		for (size_t i = 0; i < light_cm.getCount(); i++)
		{
			auto entityHandle = light_cm.getEntityHandle(i);
			auto& transform = transform_cm.getComponent_unsafe(entityHandle);

			/*Vector3f translation;
			Vector3f scale;
			Vector3f rotation;
			
			translation = transform.translation;
			rotation = transform.rotation;
			scale = transform.scale * 0.5f;*/

			//MyMath::decompose(transform.worldMatrix, scale, rotation, translation);
			//
			//translation = translation + sphereCollider.center;
			//scale = Vector3f(1.0f, 1.0f, 1.0f) * MyMath::max(scale.x, MyMath::max(scale.y, scale.z))
			//	* sphereCollider.radius * (toModelScaleBaseRatio);		// model 2r = 2m

			//Matrix4f ret_transform = MyMath::translate(translation) * MyMath::eulerToMatrix4f(rotation) * MyMath::scale(scale);
			//RendererDebug::drawDebugSphere(ret_transform, color, thickness);

			auto radius = MyMath::max(transform.scale.x, transform.scale.z) /** (capsuleCollider.radius)*/;
			auto normal = MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 0.0f, 1.0f);

			Vector3f translation = transform.translation;
			Vector3f scale = Vector3f(1.0f, 1.0f, 1.0f) * radius;		// model 2r = 2m
			Vector3f rotation = {};

			auto height = 4.0f;

			auto half_height = height * 0.5f * transform.scale.y;
			auto base_center = translation;
			auto tip_center = translation + normal * (half_height - radius);

			Matrix4f ret_base_transform = MyMath::translate(base_center) * MyMath::eulerToMatrix4f(transform.rotation) * MyMath::scale(scale);
			Matrix4f ret_tip_transform = MyMath::translate(tip_center) * MyMath::eulerToMatrix4f(transform.rotation) * MyMath::scale(scale);

			RendererDebug::drawDebugSphere(ret_base_transform, color, thickness);
			RendererDebug::drawDebugSphere(ret_tip_transform, color, thickness);
		}
	}

	void vog::MocapLayer::updateMocap(float dt_)
	{
#ifdef VOG_ENABLE_KINECT
		if (m_pKinectAPI)
		{
			m_pKinectAPI->onUpdate();
		}
		if (m_pKinectModel)
		{
			//m_modelEntity
			m_pKinectAPI->onUpdateBonesTransfom(m_pKinectModel);
		}

		{
			m_arduinoSerial.update();
			//m_moveEntity.getComponent<TransformComponent>().rotation = MyMath::toEulerAngles(m_arduinoSerial.getResultData()[0].orienatation);

			Entity entity = { m_rightSaberHandle, m_pActiveScene.get() };
			auto* pTransform = &entity.getComponent<TransformComponent>();
			const Vector3f& right_arm_rotation = m_arduinoSerial.getFinalRotation(IMUJointType::Right_Arm);
			pTransform->rotation.x = right_arm_rotation.z;
			pTransform->rotation.z = right_arm_rotation.x;
			pTransform->rotation.y = right_arm_rotation.y;

			if (m_pKinectAPI->getJointUpdatedData()[JointType_HandRight].isTrakced)
			{
				pTransform->translation = m_pKinectAPI->getFinalData_Poisition(JointType_HandRight);
			}

			/*VOG_LOG_INFO("== Update Start==");
			VOG_LOG_INFO("right arm rotation");
			VOG_LOG_GLM(pTransform->rotation);*/

			entity = { m_leftSaberHandle, m_pActiveScene.get() };
			pTransform = &entity.getComponent<TransformComponent>();
			const Vector3f& left_arm_rotation = m_arduinoSerial.getFinalRotation(IMUJointType::Left_Arm);

			pTransform->rotation.x = -left_arm_rotation.z;
			pTransform->rotation.z = left_arm_rotation.x;
			pTransform->rotation.y = left_arm_rotation.y;

			if (m_pKinectAPI->getJointUpdatedData()[JointType_HandLeft].isTrakced)
			{
				pTransform->translation = m_pKinectAPI->getFinalData_Poisition(JointType_HandLeft);
			}

			//VOG_LOG_INFO("left arm rotation");
			//VOG_LOG_GLM(pTransform->rotation);
			//VOG_LOG_INFO("== Update End==");
		}
#endif // VOG_ENABLE_KINECT
	}

	Vector2f MocapLayer::getViewportSize()
	{
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		Vector2f m_ViewportBounds[2];
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		Vector2f viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];

		return viewportSize;
	}

	Vector2f MocapLayer::getViewportMousePosition()
	{
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();

		Vector2f m_ViewportBounds[2];
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		Vector2f mouse;
		mouse.x = ImGui::GetMousePos().x;
		mouse.y = ImGui::GetMousePos().y;
		mouse -= m_ViewportBounds[0];
		return mouse;
	}

	void MocapLayer::onDebugRender()
	{
		RendererDebug::setScreenMap(DeferredRenderer::getReusltID(0));

		RendererDebug::beginScene(m_cameraController.getCamera());

		if (m_isEnableColliderOutline)
			renderColliderOutline();

		if (m_isEnableLightOutline)
			renderLightOutline();

		RendererDebug::drawAxis_XYZ();

		// TODO: temp
		RendererDebug::drawLine(m_ray.origin, m_ray.origin + m_ray.direction * 9999.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
		//m_ray.isDrawn = false;

		RendererDebug::endScene();
	}

	bool MocapLayer::onMouseButtonPressed(MouseButtonPressedEvent& event_)
	{
		if (event_.getButton() == static_cast<int>(KeyCode::MButton))
		{
			static bool isReloaded = false;

			AssetManager::reloadShaders();
		}
		if (event_.getButton() == static_cast<int>(KeyCode::MButton))
		{
			static bool isMultiSample = false;
			RenderCommand::setEnableMutliSample(isMultiSample);
			isMultiSample = !isMultiSample;
		}

		return false;
	}

	bool MocapLayer::onKeyPressed(KeyPressedEvent& event_)
	{
		if (event_.getKeyCode() == KeyCode::P)
		{
			if (m_sceneState == EditorSceneState::Edit)
			{
				onScenePlay();
			}
			else if (m_sceneState == EditorSceneState::Play)
			{
				onSceneStop();
			}
		}
		return false;
	}

}