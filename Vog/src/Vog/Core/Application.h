#pragma once

#include "Vog/Core/Core.h"
#include "Vog/Core/Window.h"
#include "Vog/Core/Timer.h"
#include "Vog/Core/LayerStack.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Events/Event.h"
#include "Vog/Events/ApplicationEvent.h"
#include "Vog/Events/MouseEvent.h"
#include "Vog/Events/KeyEvent.h"

#include "Vog/ImGui/ImGuiLayer.h"

#include "Vog/Math/MyMath.h"

namespace vog {

	class VOG_API Application : public NonCopyable
	{
	public:
		Application(const std::string& name_ = "Vog Engine App");
		virtual ~Application();

		void pushLayer(Layer* layer_);
		void pushOverlayer(Layer* overlayer_);

		void run();
		void close();

		void onEvent(Event& event_);

		inline static Application& get() { return *s_instance; }
		inline ImGuiLayer& getImGuiLayer() { return *m_pImguiLayer; }

		inline Window& getWindow() { return *m_pWindow; }

	private:
		bool _onWindowClose(WindowCloseEvent& event_);
		bool _onWindowResize(WindowResizeEvent& event_);

	private:
		static Application* s_instance;

		bool m_isRunning = true, m_isMinimized = false;

		ScopePtr<Window> m_pWindow;
		LayerStack m_layerStack;

		ImGuiLayer* m_pImguiLayer;

		float m_lastFrameTime = 0.0f;
	};

	Application* createApplication();

}