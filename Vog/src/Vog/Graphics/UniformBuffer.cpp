#include "vogpch.h"
#include "UniformBuffer.h"

#include "Vog/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace vog {

	RefPtr<UniformBuffer> UniformBuffer::create(uint32_t sizeOfByte_, uint32_t binding_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLUniformBuffer>(sizeOfByte_, binding_);
		}
		return nullptr;
	}
}