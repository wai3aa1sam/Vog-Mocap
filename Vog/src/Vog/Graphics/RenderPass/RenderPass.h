#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/Material.h"
#include "Vog/Graphics/Framebuffer.h"

namespace vog {

	class VOG_API RenderPass : public NonCopyable
	{
	public:
		RenderPass();
		~RenderPass();

		void init(RefPtr<Framebuffer>& pFramebuffer_, RefPtr<Shader>& pShader_, RefPtr<VertexArray>& pVertexArray_);
		void setVertexLayout(const VertexLayout& vertexLayout_);
		void bind();

		void onWindowResize(uint32_t width_, uint32_t height_);

		RefPtr<Shader>& getShader();
		RefPtr<VertexArray>& getVertexArray();
		RefPtr<Framebuffer>& getFramebuffer();

		uint32_t getResultID(uint32_t index_ = 0);

	private:
		RefPtr<Framebuffer> m_pFramebuffer;

		// pipeline
		RefPtr<Shader> m_pShader;
		RefPtr<VertexArray> m_pVertexArray;
	};

	inline RefPtr<Shader>& RenderPass::getShader() { return m_pShader; }
	inline RefPtr<VertexArray>& RenderPass::getVertexArray() { return m_pVertexArray; }
	inline RefPtr<Framebuffer>& RenderPass::getFramebuffer() { return m_pFramebuffer; }

	inline uint32_t RenderPass::getResultID(uint32_t index_) { return m_pFramebuffer->getColorAttachmentID(index_); }

}