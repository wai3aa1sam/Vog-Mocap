#include "vogpch.h"
#include "Vog/Graphics/RenderPass/RenderPass.h"

namespace vog {
	RenderPass::RenderPass()
	{
	}

	RenderPass::~RenderPass()
	{
		m_pFramebuffer.reset();
		m_pShader.reset();
		m_pVertexArray.reset();
	}

	void RenderPass::init(RefPtr<Framebuffer>& pFramebuffer_, RefPtr<Shader>& pShader_, RefPtr<VertexArray>& pVertexArray_)
	{
		m_pFramebuffer = pFramebuffer_;
		m_pShader = pShader_;
		m_pVertexArray = pVertexArray_;
	}

	void RenderPass::setVertexLayout(const VertexLayout& vertexLayout_)
	{
		m_pVertexArray->bind();
		m_pVertexArray->bindVertexAttributes(vertexLayout_);
	}

	void RenderPass::bind()
	{
		m_pFramebuffer->bind();
	}

	void RenderPass::onWindowResize(uint32_t width_, uint32_t height_)
	{
		m_pFramebuffer->resize(width_, height_);
	}
}