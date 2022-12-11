#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Framebuffer.h"

namespace vog {

	class VOG_API OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& specification_);
		virtual ~OpenGLFramebuffer();
		virtual void bind() override;
		virtual void unbind() override;

		virtual void resize(uint32_t width_, uint32_t height_) override;
		virtual void blit(uint32_t dst_rendererID_, uint32_t dst_Width_, uint32_t dst_height_, BufferBitType type_) override;

		virtual inline uint32_t getRendererID() const override { return m_rendererID; };

		inline virtual const uint32_t getColorAttachmentID(uint32_t index_ = 0) const override
		{ 
			VOG_CORE_ASSERT(index_ < m_colorAttachmentIDs.size(), "");
			VOG_CORE_ASSERT(m_colorAttachmentIDs[index_] != 0, "color attachment is invalid");
			return m_colorAttachmentIDs[index_];
		};
		inline virtual const uint32_t getDepthAttachmentID() const override { VOG_CORE_ASSERT(m_depthAttachmentID != 0, "depth attachment is invalid"); return m_depthAttachmentID; };

		inline virtual const FramebufferSpecification& getSpecification() const override { return m_specification; };

		inline virtual uint32_t getColorAttachmentCount() const override { return (uint32_t)m_colorAttachmentSpecifications.size(); };

	private:
		void _invalidate();

	private:
		uint32_t m_rendererID = 0;
		FramebufferSpecification m_specification;

		std::vector<FramebufferTextureSpecification> m_colorAttachmentSpecifications;
		FramebufferTextureSpecification m_depthSpecification;

		std::vector<uint32_t> m_colorAttachmentIDs;
		uint32_t m_depthAttachmentID = 0;

	};
}