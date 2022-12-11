#include "vogpch.h"
#include "Vog/Graphics/Texture.h"
#include "Vog/Graphics/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"
namespace vog {

#pragma region Texture2D
	RefPtr<Texture2D> Texture2D::create(uint32_t width_, uint32_t height_, const TextureSpecification& specification_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLTexture2D>(width_, height_, specification_);
		}
		return nullptr;
	}
	RefPtr<Texture2D> Texture2D::create(const std::string& path_, const TextureSpecification& specification_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLTexture2D>(path_, specification_);
		}
		return nullptr;
	}

	void Texture2D::bind(uint32_t rendererID_, uint32_t slot_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!");		return;
			case GraphicsAPI::API::OpenGL:  OpenGLTexture2D::bind(rendererID_, slot_);										return;
		}
	}
#pragma endregion
	RefPtr<Cubemap> Cubemap::create(uint32_t size_, const TextureSpecification& specification_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLCubemap>(size_, specification_);
		}
		return nullptr;
	}
	RefPtr<Cubemap> Cubemap::create(const std::string& fileDirectory_, const TextureSpecification& specification_)
	{
		switch (Renderer::getGraphicsAPI())
		{
			case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLCubemap>(fileDirectory_, specification_);
		}
		return nullptr;
	}
}