#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Texture.h"
#include <glad/glad.h>

namespace vog {

	class VOG_API OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width_, uint32_t height_, const TextureSpecification& specification_);
		OpenGLTexture2D(const std::string& filepath_, const TextureSpecification& specification_);

		virtual ~OpenGLTexture2D();

		virtual void bind(uint32_t slot_ = 0) override;
		virtual void setData(void* pData_, size_t size_) override;

		virtual void unbind() override;

		virtual inline uint32_t getWidth() const override { return m_width; };
		virtual inline uint32_t getHeight() const override { return m_height; };
		virtual inline uint32_t getRendererID() const override { return m_rendererId; };

		static void bind(uint32_t rendererID_, uint32_t slot_ = 0);

	private:
		uint32_t m_rendererId = 0;
		uint32_t m_width = 0, m_height = 0;
		GLenum m_internalFormat, m_dataFormat;
	};

	class VOG_API OpenGLCubemap : public Cubemap
	{
	public:
		OpenGLCubemap(uint32_t size_, const TextureSpecification& specification_);
		OpenGLCubemap(const std::filesystem::path& fileDirectory_, const TextureSpecification& specification_);

		virtual ~OpenGLCubemap();

		virtual void bind(uint32_t slot_ = 0) override;
		virtual void setData(void* pData_, size_t size_) override;
		
		//virtual void resize(void* pData_, size_t size_) override;

		virtual void unbind() override;

		virtual inline uint32_t getWidth() const override { return m_size; };
		virtual inline uint32_t getHeight() const override { return m_size; };
		virtual inline uint32_t getRendererID() const override { return m_rendererId; };

	private:
		uint32_t m_rendererId = 0;
		uint32_t m_size = 0;
		GLenum m_internalFormat, m_dataFormat;
	};
}