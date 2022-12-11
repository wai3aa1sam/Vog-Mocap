#include "vogpch.h"
#include "Vog/Graphics/RenderCommand.h"

#include "Platform/OpenGL/OpenGLGraphicsAPI.h"

namespace vog {

	// later will change when using other Graphics API
	ScopePtr<GraphicsAPI> RenderCommand::s_pInstance = createScopePtr<OpenGLGraphicsAPI>();
}
