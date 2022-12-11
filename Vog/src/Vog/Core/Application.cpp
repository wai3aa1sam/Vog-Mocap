#include "vogpch.h"

#include "Vog/Core/Application.h"

#include "Vog/Graphics/Renderer/DeferredRenderer.h"
#include "Vog/Graphics/Renderer/Renderer.h"
#include "Vog/Graphics/Renderer/RendererInitData.h"

#include "Vog/Debug/RendererDebug.h"
//#include "Vog/Debug/Gizmos.h"

#include "Vog/Resources/AssetManager.h"

namespace vog {

	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& name_)
	{
		s_instance = this;

		m_pWindow = ScopePtr<Window>(Window::create(name_));
		m_pWindow->setAppOnEventCallBackFn(BIND_EVENT_CALLBACK(Application::onEvent));
		m_pWindow->setVsync(true);

		m_pImguiLayer = new ImGuiLayer();
		pushOverlayer(m_pImguiLayer);

		//Gizmos::init();
		AssetManager::init();

		{	// Renderer init
			RenderCommand::init();

			RefPtr<UniformBuffer> pCameraUniformBuffer = CameraUniformBuffer::createUniformBuffer();
			RendererDebug::init(pCameraUniformBuffer);
			Renderer::init(pCameraUniformBuffer);
			DeferredRenderer::init(pCameraUniformBuffer);
		}
	}

	Application::~Application()
	{
		m_layerStack.destroy();

		AssetManager::shuntdown();
		//Gizmos::shuntdown();

		{	// Renderer shutdown
			DeferredRenderer::shuntdown();
			RendererDebug::shuntdown();
			Renderer::shuntdown();

			RenderCommand::shutdown();
		}
	}

	void Application::pushLayer(Layer* layer_)
	{
		m_layerStack.pushLayer(layer_);
		layer_->onAttach();
	}

	void Application::pushOverlayer(Layer* overlayer_)
	{
		m_layerStack.pushOverlayer(overlayer_);
		overlayer_->onAttach();
	}

	void Application::onEvent(Event& event_)
	{
		EventDispatcher dispatcher(event_);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_CALLBACK(Application::_onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_CALLBACK(Application::_onWindowResize));

		for (auto& rLayer : m_layerStack.revEach())
		{
			//VOG_CORE_LOG_TRACE(rLayer->getName());
			if (event_.isHandled)
			{
				break;
			}
			rLayer->onEvent(event_);
		}
	}

	void Application::run()
	{
		//GLint isMultiSample = 0;
		//glGetIntegerv(GL_SAMPLE_BUFFERS, &isMultiSample);
		//int texture_units = 0;
		//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
		//std::cout << "max texture size: " << texture_units << std::endl;
		//isMultiSample ? std::cout << "able to multi sample\n" : std::cout << "unable to multi sample\n";

		while (m_isRunning)
		{
			float time = Timer::getTime();
			float dt = time - m_lastFrameTime;
			m_lastFrameTime = time;
			
			//VOG_CORE_LOG_TRACE("Timestep: {0}s,    {1}ms", timestep, timestep * 0.001f);
			//VOG_CORE_LOG_TRACE("onUpdate");

			for (auto& layer : m_layerStack)
			{
				layer->onUpdate(dt);
			}

			m_pImguiLayer->begin();

			for (auto& layer : m_layerStack)
			{
				layer->onImGuiRender();
			}

			m_pImguiLayer->end();

			m_pWindow->onUpdate();
		}
	}

	void Application::close()
	{
		m_isRunning = false;
	}

	bool Application::_onWindowClose(WindowCloseEvent& event_)
	{
		close();
		return true;
	}

	bool Application::_onWindowResize(WindowResizeEvent& event_)
	{
		uint32_t width = event_.getWidth(), height = event_.getHeight();
		if (width == 0 || height == 0)
		{
			m_isMinimized = true;
			return false;
		}

		m_isMinimized = false;

		//Renderer::onWindowResize(width, height);
		return false;
	}
}