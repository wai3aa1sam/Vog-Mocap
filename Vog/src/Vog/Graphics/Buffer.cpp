#include "vogpch.h"
#include "Vog/Graphics/Buffer.h"
#include "Vog/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace vog {
	RefPtr<VertexBuffer> VertexBuffer::create(uint32_t totalByteSize_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLVertexBuffer>(totalByteSize_);
		}
		return nullptr;
	}
	RefPtr<VertexBuffer> VertexBuffer::create(const void* vertices_, uint32_t totalByteSize_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLVertexBuffer>(vertices_, totalByteSize_);
		}
		return nullptr;
	}

	RefPtr<IndexBuffer> IndexBuffer::create(const uint32_t* indices_, uint32_t count_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLIndexBuffer>(indices_, count_);
		}
		return nullptr;
	}
}
