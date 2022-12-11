#include "vogpch.h"
#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace vog {
    RefPtr<VertexArray> VertexArray::create()
    {
        switch (Renderer::getGraphicsAPI())
        {
        case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
        case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLVertexArray>();
        }
        return nullptr;
    }

    RefPtr<VertexArray> VertexArray::create(const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_)
	{
        switch (Renderer::getGraphicsAPI())
        {
            case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
            case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLVertexArray>(pVertexBuffer_, pIndexBuffer_);
        }
        return nullptr;
	}

}