#include "vogpch.h"
#include "Vog/Graphics/Framebuffer.h"
#include "Vog/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace vog {
    RefPtr<Framebuffer> Framebuffer::create(const FramebufferSpecification& specification_)
    {
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLFramebuffer>(specification_);
		}
		return nullptr;
    }
}