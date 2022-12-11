#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "GraphicsAPI.h"

#include <vector>

namespace vog {

	enum class FramebufferTextureFormat
	{
		None = 0,
		// Color
		RGBA8,
		RGBA16F,
		RGBA32F,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,
		DEPTH32F,

		Depth = DEPTH24STENCIL8,
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat textureFormat_)
			:
			textureFormat(textureFormat_)
		{}

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments_)
			:
			attachments(attachments_)
		{

		}
		std::vector<FramebufferTextureSpecification> attachments;
	};

	struct VOG_API FramebufferSpecification
	{
		uint32_t width = 0, height = 0;
		FramebufferAttachmentSpecification attachment;
		uint32_t samples = 1;
	};

	class VOG_API Framebuffer : public NonCopyable
	{
	public:
		Framebuffer() = default;
		virtual ~Framebuffer() = default;
		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width_, uint32_t height_) = 0;

		virtual void blit(uint32_t dst_rendererID_, uint32_t dst_Width_, uint32_t dst_height_, BufferBitType type_) = 0;

		virtual inline uint32_t getRendererID() const = 0;

		inline virtual const uint32_t getColorAttachmentID(uint32_t index_ = 0) const = 0;
		inline virtual const uint32_t getDepthAttachmentID() const = 0;
		inline virtual const FramebufferSpecification& getSpecification() const = 0;

		inline virtual uint32_t getColorAttachmentCount() const = 0;

		static RefPtr<Framebuffer> create(const FramebufferSpecification& specification_);
	private:
	};
}