#pragma once
#include <Vog.h>
#include <ImGui/imgui.h>

#include "Vog/ImGui/ImGuiLibrary.h"
#include "Vog/Editor/ScenePanel.h"

// for motcap
#include "Kinect/KinectAPI.h"
#include "Serial/ArduinoSerial.h"

// test feature
#include "Vog/Graphics/Features/Trail.h"

// demo game
#include "DemoGame/Boundary.h"
#include "DemoGame/ColorType.h"
#include "DemoGame/LightManager.h"
#include "DemoGame/Player.h"
#include "DemoGame/RhythmCube.h"
#include "DemoGame/RhythmCubeManager.h"
#include "DemoGame/Saber.h"
#include "DemoGame/SaberTrail.h"
#include "DemoGame/ScoreManager.h"

namespace vog
{
	class MocapLayer : public Layer
	{
	public:
		MocapLayer(const std::string& name_);

		virtual ~MocapLayer();

		virtual void onAttach() override;
		virtual void onDetach() override;
		virtual void onUpdate(float timestep_) override;
		virtual void onEvent(Event& event_) override;
		virtual void onImGuiRender() override;
		virtual void onDebugRender() override;

		void onScenePlay();
		void onSceneStop();

		void renderColliderOutline();
		void renderLightOutline();

		void updateMocap(float dt_);

		Vector2f getViewportSize();
		Vector2f getViewportMousePosition();

		bool onMouseButtonPressed(MouseButtonPressedEvent& event_);
		bool onKeyPressed(KeyPressedEvent& event_);

	private:
		CameraController m_cameraController;

		Vector2f m_viewportSize = { 0.0f , 0.0f };
		bool m_isViewportFocused = false, m_isViewportHovered = false;
		
		//Scene
		RefPtr<Scene> m_pEditorScene;
		RefPtr<Scene> m_pActiveScene;

		enum class EditorSceneState
		{
			Edit = 0, Play,
		};
		EditorSceneState m_sceneState = EditorSceneState::Edit;

		ScenePanel m_scenePanel;
		bool m_isEnableColliderOutline = true;
		bool m_isEnableLightOutline = true;

		RefPtr<Texture2D> m_pRedTexture;
		RefPtr<Texture2D> m_pBlueTexture;
		RefPtr<Texture2D> m_pWhiteTexture;

		// test
		RefPtr<Cubemap> m_pCubemap;
		Entity m_testCubeEntity;

		struct ray
		{
			Vector3f origin;
			Vector3f direction;
		};
		ray m_ray;

		Entity m_moveEntity;

		// for mocap
		ArduinoSerial m_arduinoSerial;
		// Kinect stuff

#ifdef VOG_ENABLE_KINECT
		ScopePtr<KinectAPI> m_pKinectAPI;
#endif // VOG_ENABLE_KINECT

		RefPtr<Model> m_pKinectModel;

		uint32_t ret = 0;

		// for demo game
		std::vector<uint32_t> m_rhythmHandles;
		std::vector<uint32_t> m_lightHandles;

		uint32_t m_rightSaberHandle;
		uint32_t m_leftSaberHandle;

		uint32_t m_modelHandle;

		Trail m_trail_left;
		Trail m_trail_right;

		demo_game::SaberTrail::Data m_saberTrailData_left;
		demo_game::SaberTrail::Data m_saberTrailData_right;
		
		bool m_isSaberAnimate = false;
	};
}