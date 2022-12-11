#include "vogpch.h"
#include "Vog/Graphics/Renderer/DeferredRenderer.h"
#include "Vog/Graphics/Renderer/RendererInitData.h"

#include "Vog/Graphics/RenderCommand.h"

#include "Vog/Resources/AssetManager.h"

#include "Vog/Objects/Light.h"

#include "Vog/ImGui/ImGuiLibrary.h"

#include <imgui.h>

namespace vog {

#pragma region RendererData
	DeferredRenderer::RendererData* DeferredRenderer::s_pData = new DeferredRenderer::RendererData;

	DeferredRenderer::RendererData::RendererData()
	{
		isInitialized = true;
	}

	DeferredRenderer::RendererData::~RendererData()
	{
		destroy();
	}

	void DeferredRenderer::RendererData::init(RefPtr<UniformBuffer>& pCameraUniformBuffer_)
	{
		VOG_CORE_ASSERT(isInitialized, "static order problem!");

		pCameraUniformBuffer = pCameraUniformBuffer_;

		FramebufferSpecification specification;
		specification.width = 1280;
		specification.height = 720;
		specification.samples = 1;
		// 0: Position, 1: Normal, 2: AlbedoSpec
		specification.attachment = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH32F };
		//auto pFramebuffer = Framebuffer::create(specification);
		//auto& pShader = AssetManager::getShader("GeometryPass");
		auto pVA = VertexArray::create();
		geometryPass.init(Framebuffer::create(specification), AssetManager::getShader("GeometryPass"), pVA);
		VertexLayout layout = {
				{ VertexDataType::Float3, "a_position",	},
				{ VertexDataType::Float3, "a_normal",	},
				{ VertexDataType::Float2, "a_texCoord",	},
				{ VertexDataType::Float3, "a_tangent",	},
		};
		geometryPass.setVertexLayout(layout);		// useless at all

		pVA = VertexArray::create();
		specification.attachment = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::DEPTH32F };
		postprocessingPass.init(Framebuffer::create(specification), AssetManager::getShader("Deferred_Postprocess"), pVA);
		postprocessingPass.setVertexLayout(layout);

		pVertexArray = VertexArray::create();

		bloom.init();
		shadow.init();

		// for develop purpose
		{
			pDeferred_postprocess_material = Material::create(AssetManager::getShader("Deferred_Postprocess"));
			if (pDeferred_postprocess_material->getLayout()->contains("u_exposure"))
				pDeferred_postprocess_material->setFloat("u_exposure", 0.1f);
		}
	}

	void DeferredRenderer::RendererData::destroy()
	{
		VOG_CORE_ASSERT(isInitialized, "static order problem!");

		isInitialized = false;
	}
#pragma endregion

	void DeferredRenderer::init(RefPtr<UniformBuffer>& pCameraUniformBuffer_)
	{
		s_pData->init(pCameraUniformBuffer_);
	}

	void DeferredRenderer::shuntdown()
	{
		delete s_pData;
		s_pData = nullptr;
	}

	void DeferredRenderer::onWindowResize(uint32_t width_, uint32_t height_)
	{
		s_pData->screenParam.x = static_cast<float>(width_);
		s_pData->screenParam.y = static_cast<float>(height_);

		//RenderCommand::setViewport(width_, height_);
		s_pData->geometryPass.onWindowResize(width_, height_);
		s_pData->postprocessingPass.onWindowResize(width_, height_);
	}

	void DeferredRenderer::beginScene(const Camera& camera_)
	{
		CameraUniformBuffer cameraUniformBuffer;
		cameraUniformBuffer.u_view = camera_.getViewMatrix();
		cameraUniformBuffer.u_projection = camera_.getProjectionMatrix();
		cameraUniformBuffer.u_viewProjection = camera_.getViewProjectionMatrix();
		cameraUniformBuffer.u_lightSpace_matrix = s_pData->shadow.getLightSpaceMatrix();
		s_pData->pCameraUniformBuffer->setData(&cameraUniformBuffer, sizeof(cameraUniformBuffer), 0);
		s_pData->cameraRotationMatrix = camera_.getViewRotationMatrix();
		s_pData->cameraPosition = camera_.getPosition();

		RenderCommand::setViewport((uint32_t)s_pData->screenParam.x, (uint32_t)s_pData->screenParam.y);
		RenderCommand::setClearColor({ 0.2f, 0.3f, 0.3f, 1.0f });
		RenderCommand::clear(BufferBitType::Color | BufferBitType::Depth);

		RenderCommand::setEnableCullFace(true);
		RenderCommand::setCullFace(CullFaceType::Back);
	}

	void DeferredRenderer::endScene()
	{
		_flush();
	}

	uint32_t DeferredRenderer::getReusltID(uint32_t index_)
	{
		//return s_pData->geometryPass.getResultID(index_);
		return s_pData->postprocessingPass.getResultID(index_);
	}

	std::pair<float, float> DeferredRenderer::getViewportSize()
	{
		return { s_pData->screenParam.x, s_pData->screenParam.y };
	}

	void DeferredRenderer::uploadLights(void* positions_, void* directions_, void* colors_, void* params_)
	{
		s_pData->pLight_positions = positions_;
		s_pData->pLight_directions = directions_;
		s_pData->pLight_colors = colors_;
		s_pData->pLight_params= params_;

		VOG_CORE_ASSERT(Light::s_current_light_count <= Light::s_max_light_count, "");
	}

	void DeferredRenderer::uploadLighViewMatrix(const Matrix4f& lightViewMatrix_)
	{
		s_pData->shadow.uploadLightViewMatrix(lightViewMatrix_);
	}

	void DeferredRenderer::submit(Submission&& submission_)
	{
		VOG_CORE_ASSERT(submission_.pMaterial, "");
		auto& pShader = submission_.pMaterial->getShader();

		if (pShader == nullptr || pShader == AssetManager::getShader("GeometryPass"))
			s_pData->deferredSubmissions.push_back(std::move(submission_));
		else
			s_pData->forwardSubmissions.push_back(std::move(submission_));
	}

	void DeferredRenderer::submit(AnimatedSubmission&& submission_)
	{
		VOG_CORE_ASSERT(submission_.pMaterial, "");
		auto& pShader = submission_.pMaterial->getShader();

		if (pShader == nullptr || pShader == AssetManager::getShader("GeometryPass_Anim"))
			s_pData->deferredAnimatedSubmissions.push_back(std::move(submission_));
		else
			s_pData->forwardAnimatedSubmissions.push_back(std::move(submission_));
	}

	void DeferredRenderer::submit(const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, const Matrix4f& transform_, const RefPtr<Material>& pMaterial_)
	{
		VOG_CORE_ASSERT(pMaterial_, "");
		auto& pShader = pMaterial_->getShader();

		Submission submission;
		submission.pVertexBuffer = pVertexBuffer_;
		submission.pIndexBuffer = pIndexBuffer_;
		submission.pMaterial = pMaterial_;
		submission.pTransform = &transform_;

		if (pShader == nullptr || pShader == s_pData->geometryPass.getShader() /*|| pShader == AssetManager::getShader("GeometryPass_Anim")*/)
			s_pData->deferredSubmissions.push_back(submission);
		else
			s_pData->forwardSubmissions.push_back(submission);
	}

	void DeferredRenderer::onImGuiRender()
	{
		ImGui::Text(s_pData->pDeferred_postprocess_material->getShader()->getName().c_str());
		s_pData->pDeferred_postprocess_material->onImGuiRender();
		
		int bloom_iteration = s_pData->bloom.getIteration();
		ImGuiLibrary::drawInputInt("bloom iteration", bloom_iteration);
		s_pData->bloom.setIteration(bloom_iteration);

		s_pData->shadow.onImGuiRender();

		//ImGui::Begin("Texture");
		
		//ImGuiLibrary::drawTextureIcon("Depth", s_pData->geometryPass.getFramebuffer()->getColorAttachmentID(), { 1280, 720 });

		//ImGui::End();
	}

	void DeferredRenderer::_flush()
	{
		// shadow pass
		{
			s_pData->shadow.process(&s_pData->deferredSubmissions, &s_pData->deferredAnimatedSubmissions, &s_pData->forwardSubmissions, &s_pData->forwardAnimatedSubmissions);
		}

		s_pData->geometryPass.bind();

		RenderCommand::setClearColor({0.0f, 0.0f, 0.0f, 0.0f});
		RenderCommand::clear(BufferBitType::Color | BufferBitType::Depth);

		RenderCommand::setDepthMask(true);
		RenderCommand::setEnableDepthTest(true);
		RenderCommand::setEnableBlending(false);

		// === geo pass
		{
			auto& pVertexArray = s_pData->geometryPass.getVertexArray();
			pVertexArray->bind();
			{
				auto& pShader = s_pData->geometryPass.getShader();
				pShader->bind();

				for (size_t i = 0; i < s_pData->deferredSubmissions.size(); i++)
				{
					auto& submission = s_pData->deferredSubmissions[i];
					VOG_CORE_ASSERT(submission.pTransform, "");

					{	// TODO: remove temp
						submission.pMaterial->setFloat("u_property.hasAlbedoMap", submission.pMaterial->hasTexture("u_albedo_map"));
						submission.pMaterial->setFloat("u_property.hasNormalMap", submission.pMaterial->hasTexture("u_normal_map"));
						submission.pMaterial->setFloat("u_property.hasSpecularMap", submission.pMaterial->hasTexture("u_specular_map"));
						submission.pMaterial->setFloat("u_property.hasEmissionMap", submission.pMaterial->hasTexture("u_emission_map"));
					}

					submission.pMaterial->uploadMaterial();
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
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer, submission.indexCount);
				}
			}

			{
				auto& pShader = AssetManager::getShader("GeometryPass_Anim");
				pShader->bind();
				for (size_t i = 0; i < s_pData->deferredAnimatedSubmissions.size(); i++)
				{
					auto& submission = s_pData->deferredAnimatedSubmissions[i];
					VOG_CORE_ASSERT(submission.pTransform, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms->size() <= s_max_n_bone, "");

					{	// TODO: remove temp
						submission.pMaterial->setFloat("u_property.hasAlbedoMap", submission.pMaterial->hasTexture("u_albedo_map"));
						submission.pMaterial->setFloat("u_property.hasNormalMap", submission.pMaterial->hasTexture("u_normal_map"));
						submission.pMaterial->setFloat("u_property.hasSpecularMap", submission.pMaterial->hasTexture("u_specular_map"));
						submission.pMaterial->setFloat("u_property.hasEmissionMap", submission.pMaterial->hasTexture("u_emission_map"));
					}
					
					submission.pMaterial->uploadMaterial();
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
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer, submission.indexCount);
				}
			}
		}

		// === Lighting pass
		{
			s_pData->postprocessingPass.bind();

			RenderCommand::setDepthMask(false);
			RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
			RenderCommand::clear(BufferBitType::Color | BufferBitType::Depth);

			auto& pShader = AssetManager::getShader("Deferred_Lighting");
			pShader->bind();
			auto& pVertexArray = s_pData->postprocessingPass.getVertexArray();
			pVertexArray->bind();

			// upload texture
			{
				std::array<std::string, 3> binding_str = { "u_position_map", "u_normal_map", "u_albedoSpec_map" };
				for (int i = 0; i < binding_str.size(); i++)
				{
					uint32_t texture_binding = pShader->getBindingLocation(binding_str[i]);
					if (texture_binding < 0)
					{
						VOG_CORE_LOG_CRITICAL("cannot find {0} texture binding!", binding_str[i]);
						break;
					}
					//VOG_CORE_ASSERT(texture_binding < 0, "");
					auto screen_texture_id = s_pData->geometryPass.getFramebuffer()->getColorAttachmentID(i);
					Texture2D::bind(screen_texture_id, texture_binding);
				}

				uint32_t texture_binding = pShader->getBindingLocation("u_shadow_map");
				auto shadowmap_texture_id = s_pData->shadow.getResultID();
				Texture2D::bind(shadowmap_texture_id, texture_binding);
			}


			pShader->setFloat4("u_screenParam", s_pData->screenParam);
			pShader->setFloat3("u_camera_position", s_pData->cameraPosition);
			pShader->setInt("u_current_light_count", Light::s_current_light_count); 
			pShader->setFloat4Array("u_light_positions",	(const Vector4f*)s_pData->pLight_positions,		Light::s_current_light_count);
			pShader->setFloat3Array("u_light_directions",	(const Vector3f*)s_pData->pLight_directions,	Light::s_current_light_count);
			pShader->setFloat4Array("u_light_colors",		(const Vector4f*)s_pData->pLight_colors,		Light::s_current_light_count);
			pShader->setFloat4Array("u_light_params",		(const Vector4f*)s_pData->pLight_params,		Light::s_current_light_count);

			auto& postprocessQuad = AssetManager::getMesh(PrimitiveMesh::PostprocessQuad);				// Plane is not vertical facing
			auto& pVertexBuffer = postprocessQuad->getpVertexBuffer();
			auto& pIndexBuffer = postprocessQuad->getpIndexBuffer();

			pIndexBuffer->bind();
			pVertexBuffer->bind();
			pVertexArray->bindVertexAttributes(pVertexBuffer->getVertexLayout());
			RenderCommand::drawIndex(Primitive::Triangle, pVertexBuffer, pIndexBuffer);
		}
#if 1
		// forward pass
		{	
			RenderCommand::setEnableBlending(true);

			RenderCommand::setDepthMask(true);
			s_pData->geometryPass.getFramebuffer()->blit(s_pData->postprocessingPass.getFramebuffer()->getRendererID(),
				(uint32_t)s_pData->screenParam.x, (uint32_t)s_pData->screenParam.y, BufferBitType::Depth);

			RenderCommand::setEnableCullFace(false);		// TODO: temp remove

			// foward render
			{	
				auto& pVertexArray = s_pData->pVertexArray;
				pVertexArray->bind();

				for (size_t i = 0; i < s_pData->forwardSubmissions.size(); i++)
				{
					auto& submission = s_pData->forwardSubmissions[i];
					VOG_CORE_ASSERT(submission.pTransform, "");

					auto& pShader = submission.pMaterial->getShader();
					pShader->bind();

					submission.pMaterial->uploadMaterial();
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
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer, submission.indexCount);
				}

				for (size_t i = 0; i < s_pData->forwardAnimatedSubmissions.size(); i++)
				{
					auto& submission = s_pData->forwardAnimatedSubmissions[i];
					VOG_CORE_ASSERT(submission.pTransform, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms, "");
					VOG_CORE_ASSERT(submission.pBoneTransforms->size() <= s_max_n_bone, "");

					auto& pShader = submission.pMaterial->getShader();
					pShader->bind();

					submission.pMaterial->uploadMaterial();
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
						RenderCommand::drawIndex(Primitive::Triangle, submission.pVertexBuffer, submission.pIndexBuffer, submission.indexCount);
				}
			}

			RenderCommand::setEnableCullFace(true);

			// skybox
			{	
				auto& pVertexArray = s_pData->pVertexArray;
				pVertexArray->bind();

				auto pShader = AssetManager::getShader("Skybox");

				RenderCommand::setEnableDepthTest(true);
				RenderCommand::setDepthFunc(DepthFuncConfig::Less_Equal);
				RenderCommand::setCullFace(CullFaceType::Front);

				pShader->bind();
				AssetManager::getDefaultCubemap()->bind();

				pShader->setMat4("u_transform", s_pData->cameraRotationMatrix);

				auto& pCubeModel = AssetManager::getMesh(PrimitiveMesh::Box);
				auto& pVertexBuffer = pCubeModel->getpVertexBuffer();
				auto& pIndexBuffer = pCubeModel->getpIndexBuffer();
				pIndexBuffer->bind();
				pVertexBuffer->bind();
				pVertexArray->bindVertexAttributes(pVertexBuffer->getVertexLayout());
				RenderCommand::drawIndex(Primitive::Triangle, pVertexBuffer, pIndexBuffer);

				RenderCommand::setDepthFunc(DepthFuncConfig::Less);
				RenderCommand::setCullFace(CullFaceType::Back);
			}

			// postprocessing
			{	
				// bloom
				auto screen_map_texture_id = s_pData->postprocessingPass.getFramebuffer()->getColorAttachmentID(1);
				auto bright_texture_id = s_pData->bloom.processBlur(screen_map_texture_id);

				// postprocess
				s_pData->postprocessingPass.bind();
				RenderCommand::setEnableDepthTest(false);
				//RenderCommand::setDepthMask(false);
				auto& pShader = s_pData->postprocessingPass.getShader();
				pShader->bind();

				{	// for develop purpose
					if (s_pData->pDeferred_postprocess_material->getLayout()->contains("u_screenParam"))
						s_pData->pDeferred_postprocess_material->setFloat4("u_screenParam", s_pData->screenParam);

					s_pData->pDeferred_postprocess_material->uploadMaterial();
					if (s_pData->pDeferred_postprocess_material->getLayout()->contains("u_exposure"))
					{
						if (s_pData->pDeferred_postprocess_material->getValue<float>("u_exposure") <= 0.0f)
							s_pData->pDeferred_postprocess_material->getValue<float>("u_exposure") = 1.0f;
					}
				}

				uint32_t texture_binding = pShader->getBindingLocation("u_screen_map");
				auto screen_texture_id = s_pData->postprocessingPass.getFramebuffer()->getColorAttachmentID();
				Texture2D::bind(screen_texture_id, texture_binding);
				texture_binding = pShader->getBindingLocation("u_bright_map");
				Texture2D::bind(bright_texture_id, texture_binding);

				pShader->setFloat4("u_screenParam", s_pData->screenParam);

				auto& postprocessQuad = AssetManager::getMesh(PrimitiveMesh::PostprocessQuad);		// Plane is not vertical facing
				auto& pVertexBuffer = postprocessQuad->getpVertexBuffer();
				auto& pIndexBuffer = postprocessQuad->getpIndexBuffer();

				auto& pVertexArray = s_pData->postprocessingPass.getVertexArray();
				pVertexArray->bind();
				pIndexBuffer->bind();
				pVertexBuffer->bind();
				pVertexArray->bindVertexAttributes(pVertexBuffer->getVertexLayout());
				RenderCommand::drawIndex(Primitive::Triangle, pVertexBuffer, pIndexBuffer);
			}

			
		}
#endif // 0

		s_pData->postprocessingPass.getFramebuffer()->unbind();

		s_pData->deferredSubmissions.clear();
		s_pData->deferredAnimatedSubmissions.clear();

		s_pData->forwardSubmissions.clear();
		s_pData->forwardAnimatedSubmissions.clear();
	}

}