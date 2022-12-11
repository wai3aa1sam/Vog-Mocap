#include "vogpch.h"
#include "Bloom.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/Graphics/RenderCommand.h"

namespace vog {

#pragma region Pingpong_pass
	void Bloom::PingpongPass::init()
	{
		FramebufferSpecification specification;
		specification.width = 1280;
		specification.height = 720;
		specification.samples = 1;
		// 0: Position, 1: Normal, 2: AlbedoSpec
		specification.attachment = { FramebufferTextureFormat::RGBA16F };

		pPingpongFB[0] = Framebuffer::create(specification);
		pPingpongFB[1] = Framebuffer::create(specification);
		pShader = AssetManager::getShader("Blur_Pingpong");
		pVertexArray = VertexArray::create();
	}

	uint32_t Bloom::PingpongPass::processBlur(uint32_t textureID_, int iteration_)
	{
		if (iteration_ < 1)
			return pPingpongFB[0]->getColorAttachmentID();
		else if (iteration_ > 20)
			iteration_ = 20;

		bool isHorizontal = true, isFirst_iteration = true;

		auto current_texture_id = textureID_;
		uint32_t texture_binding = pShader->getBindingLocation("u_screen_map");

		auto& postprocessQuad = AssetManager::getMesh(PrimitiveMesh::PostprocessQuad);		// Plane is not vertical facing
		auto& pVertexBuffer = postprocessQuad->getpVertexBuffer();
		auto& pIndexBuffer = postprocessQuad->getpIndexBuffer();
		pVertexArray->bind();
		pIndexBuffer->bind();
		pVertexBuffer->bind();
		pVertexArray->bindVertexAttributes(pVertexBuffer->getVertexLayout());

		pShader->bind();

		for (int i = 0; i < iteration_; i++)
		{
			pPingpongFB[isHorizontal]->bind();

			if (!isFirst_iteration)
				current_texture_id = pPingpongFB[!isHorizontal]->getColorAttachmentID();

			Texture2D::bind(current_texture_id, texture_binding);
			pShader->setFloat("u_isHorizontal", (float)isHorizontal);

			auto& postprocessQuad = AssetManager::getMesh(PrimitiveMesh::PostprocessQuad);		// Plane is not vertical facing

			//pVertexBuffer->bind();
			RenderCommand::drawIndex(Primitive::Triangle, pVertexBuffer, pIndexBuffer);

			isHorizontal = !isHorizontal;

			if (isFirst_iteration)
				isFirst_iteration = false;
		}
		//pPingpongFB[!isHorizontal]->unbind();

		

		return pPingpongFB[!isHorizontal]->getColorAttachmentID();
	}

#pragma endregion

	Bloom::Bloom()
	{
	}

	Bloom::~Bloom()
	{
	}

	void Bloom::init()
	{
		m_pingpongPass.init();
	}

	uint32_t Bloom::processBlur(uint32_t textureID_)
	{
		return m_pingpongPass.processBlur(textureID_, m_iteration);
	}
	
}