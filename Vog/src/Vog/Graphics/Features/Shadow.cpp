#include "vogpch.h"
#include "Shadow.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/Graphics/RenderCommand.h"

#include "Vog/ImGui/ImGuiLibrary.h"

namespace vog {

#pragma region Shadow::ShadowMapping
	void Shadow::ShadowMapping::init()
	{
		FramebufferSpecification specification;
		specification.width = size;
		specification.height = size;
		specification.samples = 1;
		specification.attachment = { FramebufferTextureFormat::DEPTH32F };
		auto pVA = VertexArray::create();
		shadowPass.init(Framebuffer::create(specification), AssetManager::getShader("Shadow_Mapping"), pVA);
	}

	void Shadow::ShadowMapping::upload(const Vector3f& position_, const Vector3f& target_)
	{
		Matrix4f lightProjection = MyMath::orthographic(-orthSize, orthSize, -orthSize, orthSize, near_plane, far_plane);
		Matrix4f lightView = MyMath::lookAt(position_, target_, Vector3f(0.0f, 1.0f, 0.0f));
		lightSpace_matrix = lightProjection * lightView;
	}

	void Shadow::ShadowMapping::uploadLightViewMatrix(const Matrix4f& viewMatrix_)
	{
		Matrix4f lightProjection = MyMath::orthographic(-orthSize, orthSize, -orthSize, orthSize, near_plane, far_plane);
		lightSpace_matrix = lightProjection * viewMatrix_;
	}

	void Shadow::ShadowMapping::process(const std::vector<Submission>* pDeferredSubmissions_, const std::vector<AnimatedSubmission>* pDeferredAnimatedSubmissions_,
		const std::vector<Submission>* pForwardSubmissions_, const std::vector<AnimatedSubmission>* pForwardAnimatedSubmissions_)
	{
		RenderCommand::setViewport(size, size);
		shadowPass.getFramebuffer()->bind();
		RenderCommand::clear(BufferBitType::Depth);

		RenderCommand::setEnableDepthTest(true);
		RenderCommand::setDepthMask(true);
		RenderCommand::setEnableCullFace(true);
		RenderCommand::setCullFace(CullFaceType::Front);

		auto& pShader = shadowPass.getShader();
		pShader->bind();
		//pShader->setMat4("u_lightSpace_matrix", lightSpace_matrix);

		auto& pVertexArray = shadowPass.getVertexArray();
		pVertexArray->bind();
		{
			// static mesh
			{
				for (size_t i = 0; i < pDeferredSubmissions_->size(); i++)
				{
					const auto& submission = (*pDeferredSubmissions_)[i];
					VOG_CORE_ASSERT(submission.pTransform, "");

					pShader->setMat4("u_transform", *submission.pTransform);

					auto& pVertexBuffer = submission.pVertexBuffer;
					auto& pIndexBuffer = submission.pIndexBuffer;
					submission.pIndexBuffer->bind();
					submission.pVertexBuffer->bind();
					pVertexArray->bindVertexAttributes(pVertexBuffer->getVertexLayout());

					auto& subMesh = submission.subMesh;
					if (subMesh.nVertices > 0)		// is a submesh
						RenderCommand::drawIndexBaseVertex(Primitive::Triangle, pVertexBuffer, pIndexBuffer, subMesh.baseVertexIndex, subMesh.baseIndex, subMesh.nIndices);
					else
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer);
				}

				for (size_t i = 0; i < pForwardSubmissions_->size(); i++)
				{
					auto& submission = (*pForwardSubmissions_)[i];
					VOG_CORE_ASSERT(submission.pTransform, "");

					pShader->setMat4("u_transform", *submission.pTransform);

					auto& pVertexBuffer = submission.pVertexBuffer;
					auto& pIndexBuffer = submission.pIndexBuffer;
					submission.pIndexBuffer->bind();
					submission.pVertexBuffer->bind();
					pVertexArray->bindVertexAttributes(submission.pVertexBuffer->getVertexLayout());

					auto& subMesh = submission.subMesh;
					if (subMesh.nVertices > 0)		// is a submesh
						RenderCommand::drawIndexBaseVertex(Primitive::Triangle, pVertexBuffer, pIndexBuffer, subMesh.baseVertexIndex, subMesh.baseIndex, subMesh.nIndices);
					else
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer);
				}
			}

			// Animation mesh
			{
				auto& pShader = AssetManager::getShader("Shadow_Mapping_Anim");
				pShader->bind();
				//pShader->setMat4("u_lightSpace_matrix", lightSpace_matrix);

				for (size_t i = 0; i < pDeferredAnimatedSubmissions_->size(); i++)
				{
					auto& submission = (*pDeferredAnimatedSubmissions_)[i];
					VOG_CORE_ASSERT(submission.pTransform, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms->size() <= s_max_n_bone, "");

					pShader->setMat4("u_transform", *submission.pTransform);

					const auto& boneTransforms = *submission.pBoneTransforms;
					pShader->setMat4Array("u_boneTransformations", boneTransforms.data(), static_cast<uint32_t>(boneTransforms.size()));

					auto& pVertexBuffer = submission.pVertexBuffer;
					auto& pIndexBuffer = submission.pIndexBuffer;
					submission.pIndexBuffer->bind();
					submission.pVertexBuffer->bind();
					pVertexArray->bindVertexAttributes(pVertexBuffer->getVertexLayout());

					auto& subMesh = submission.subMesh;
					if (subMesh.nVertices > 0)		// is a submesh
						RenderCommand::drawIndexBaseVertex(Primitive::Triangle, pVertexBuffer, pIndexBuffer, subMesh.baseVertexIndex, subMesh.baseIndex, subMesh.nIndices);
					else
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer);
				}

				for (size_t i = 0; i < pForwardAnimatedSubmissions_->size(); i++)
				{
					auto& submission = (*pForwardAnimatedSubmissions_)[i];
					VOG_CORE_ASSERT(submission.pTransform, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms->size() <= s_max_n_bone, "");

					pShader->setMat4("u_transform", *submission.pTransform);

					const auto& boneTransforms = *submission.pBoneTransforms;
					pShader->setMat4Array("u_boneTransformations", boneTransforms.data(), static_cast<uint32_t>(boneTransforms.size()));

					auto& pVertexBuffer = submission.pVertexBuffer;
					auto& pIndexBuffer = submission.pIndexBuffer;
					submission.pIndexBuffer->bind();
					submission.pVertexBuffer->bind();
					pVertexArray->bindVertexAttributes(submission.pVertexBuffer->getVertexLayout());

					auto& subMesh = submission.subMesh;
					if (subMesh.nVertices > 0)		// is a submesh
						RenderCommand::drawIndexBaseVertex(Primitive::Triangle, pVertexBuffer, pIndexBuffer, subMesh.baseVertexIndex, subMesh.baseIndex, subMesh.nIndices);
					else
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer);
				}
			}
		}

		RenderCommand::setCullFace(CullFaceType::Back);
	}

	uint32_t Shadow::ShadowMapping::getResultID()
	{
		return shadowPass.getFramebuffer()->getDepthAttachmentID();
	}

	const Matrix4f& Shadow::ShadowMapping::getLightSpaceMatrix()
	{
		return lightSpace_matrix;
	}

	void Shadow::ShadowMapping::onImGuiRender()
	{
		ImGuiLibrary::drawText("Shadow map params");
		ImGuiLibrary::drawDragFloat("orthsize", orthSize);
		ImGuiLibrary::drawDragFloat("near plane", near_plane);
		ImGuiLibrary::drawDragFloat("far plane", far_plane);

	}
#pragma endregion


	void Shadow::init()
	{
		m_shadowMapping.init();
	}

	void Shadow::upload(const Vector3f& position_, const Vector3f& target_)
	{
		m_shadowMapping.upload(position_, target_);
	}

	void Shadow::uploadLightViewMatrix(const Matrix4f& viewMatrix_)
	{
		m_shadowMapping.uploadLightViewMatrix(viewMatrix_);
	}

	void Shadow::process(const std::vector<Submission>* pDeferredSubmissions_, const std::vector<AnimatedSubmission>* pDeferredAnimatedSubmissions_, const std::vector<Submission>* pForwardSubmissions_, const std::vector<AnimatedSubmission>* pForwardAnimatedSubmissions_)
	{
		m_shadowMapping.process(pDeferredSubmissions_, pDeferredAnimatedSubmissions_, pForwardSubmissions_, pForwardAnimatedSubmissions_);
	}

	void Shadow::onImGuiRender()
	{
		m_shadowMapping.onImGuiRender();
	}

	const Matrix4f& Shadow::getLightSpaceMatrix()
	{
		return m_shadowMapping.getLightSpaceMatrix();
	}
	uint32_t Shadow::getResultID()
	{
		return m_shadowMapping.getResultID();
	}
}