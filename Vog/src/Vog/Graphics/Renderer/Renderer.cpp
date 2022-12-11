#include "vogpch.h"
#include "Vog/Graphics/Renderer/Renderer.h"
#include "Vog/Graphics/Renderer/RendererInitData.h"

#include "Vog/Graphics/Material.h"
#include "Vog/Graphics/RenderCommand.h"

#include "Vog/Resources/AssetManager.h"

#include <glad/glad.h>		// TODO: remove

namespace vog {

	Renderer::RendererData* Renderer::s_pData = new Renderer::RendererData;

#pragma region RendererData

	Renderer::RendererData::RendererData()
	{
		isInitialized = true;
	}

	Renderer::RendererData::~RendererData()
	{
		destroy();
	}

	void Renderer::RendererData::init(RefPtr<UniformBuffer>& pCameraUniformBuffer_)
	{
		VOG_CORE_ASSERT(isInitialized, "static order problem!");

		pCameraUniformBuffer = pCameraUniformBuffer_;
		pVertexArray = VertexArray::create();

		shaderLibrary.load("assets/shaders/texture.glsl");
		shaderLibrary.load("assets/shaders/vertexPos.glsl");
		shaderLibrary.load("assets/shaders/Animation.glsl");
		shaderLibrary.load("assets/shaders/Simple_Model_Loading.glsl");
		shaderLibrary.load("assets/shaders/Skybox.glsl");

		pCurrentShader = shaderLibrary.getShader("Simple_Model_Loading");
	}

	void Renderer::RendererData::destroy()
	{
		VOG_CORE_ASSERT(isInitialized, "static order problem!");

		pCameraUniformBuffer.reset();
		shaderLibrary.destroy();

		pCurrentShader.reset();
		pVertexArray.reset();

		isInitialized = false;
	}
#pragma endregion


	void Renderer::init(RefPtr<UniformBuffer>& pCameraUniformBuffer_)
	{
		s_pData->init(pCameraUniformBuffer_);
	}

	void Renderer::shuntdown()
	{
		delete s_pData;
		s_pData = nullptr;
	}

	void Renderer::onWindowResize(uint32_t width_, uint32_t height_)
	{
		RenderCommand::setViewport(width_, height_);
	}

	void Renderer::beginScene(const Camera& camera_)
	{
		CameraUniformBuffer cameraUniformBuffer;
		cameraUniformBuffer.u_view = camera_.getViewMatrix();
		cameraUniformBuffer.u_projection = camera_.getProjectionMatrix();
		cameraUniformBuffer.u_viewProjection = camera_.getViewProjectionMatrix();
		s_pData->pCameraUniformBuffer->setData(&cameraUniformBuffer, sizeof(cameraUniformBuffer), 0);
		s_pData->cameraRotationMatrix = camera_.getViewRotationMatrix();
	}

	void Renderer::endScene()
	{
		s_pData->shaderLibrary.endScene();
	}

	void Renderer::submit(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_)
	{
		pShader_->bind();
		pShader_->setMat4("u_transform", transform_);

		pVertexArray_->bind();
		RenderCommand::drawIndex(Primitive::Triangle, pVertexArray_->getVertexBuffer(), pVertexArray_->getIndexBuffer());
	}

	void Renderer::submit(const RefPtr<Model>& pModel_, float dt_, const Matrix4f& transform_)
	{
		if (!pModel_ || pModel_->m_materialPtrs.size() < 1)
			return;

		auto pShader = pModel_->m_materialPtrs[0]->getShader();

		if (pModel_->isAnimation())
		{
			pShader->bind();

			pModel_->onUpdateAnimation(dt_);
			const auto& transforms = pModel_->getBoneTransforms();
			pShader->setMat4Array("u_boneTransformations", transforms.data(), 100);
		}
		auto& pVertexArray = s_pData->pVertexArray;
		pVertexArray->bind();

		s_pData->pVertexArray->setVertexBuffer(pModel_->m_pVertexArray->getVertexBuffer());
		s_pData->pVertexArray->setIndexBuffer(pModel_->m_pVertexArray->getIndexBuffer());

		///auto& pVertexArray = pModel_->m_pVertexArray;

		const auto& texturePtrs = pModel_->m_texturePtrs;

		//VOG_CORE_LOG_INFO("=========Start Model============");

		for (auto& subMesh : pModel_->m_subMeshes)
		{
			//VOG_CORE_LOG_INFO("mesh name: {0}", subMesh.m_name);
			//VOG_CORE_LOG_INFO("baseIndex: {0}, nVertices: {1}, baseIndex: {2}, nIndices: {3}, materialIndex: {4}, ", 
			//	subMesh.baseVertexIndex, subMesh.nVertices, subMesh.baseIndex, subMesh.nIndices, subMesh.materialIndex);

			auto& material = *pModel_->m_materialPtrs[subMesh.materialIndex];
			material.uploadMaterial();

			pShader->setMat4("u_transform", transform_);

			RenderCommand::drawIndexBaseVertex(Primitive::Triangle, pModel_->m_pVertexArray->getVertexBuffer(), pModel_->m_pVertexArray->getIndexBuffer(),
				subMesh.baseVertexIndex, subMesh.baseIndex, subMesh.nIndices);
		}
	}

	void Renderer::submitLines(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_)
	{
		pShader_->bind();
		pShader_->setMat4("u_transform", transform_);

		pVertexArray_->bind();
		RenderCommand::drawIndex(Primitive::Line, pVertexArray_->getVertexBuffer(), pVertexArray_->getIndexBuffer());
	}

	void Renderer::submitMesh(const MeshComponent& meshComponent_, const MeshRendererComponent& meshRendererComponent_, const Matrix4f& transform_)
	{
		VOG_CORE_ASSERT(meshRendererComponent_.pMaterial, "");

		if (s_pData->pVertexArray->getVertexBuffer() != meshComponent_.pVertexBuffer)
		{
			s_pData->pVertexArray->setVertexBuffer(meshComponent_.pVertexBuffer);
			s_pData->pVertexArray->setIndexBuffer(meshComponent_.pIndexBuffer);
		}

		auto& pMaterial = meshRendererComponent_.pMaterial;
		auto& pShader = pMaterial->getShader();

		//auto& material = *meshRendererComponent_.pMaterial;
		const auto& subMesh = meshComponent_.subMesh;

		if (subMesh.nVertices > 0)
		{
			//VOG_CORE_LOG_INFO("mesh name: {0}", subMesh.name);
			//VOG_CORE_LOG_INFO("baseIndex: {0}, nVertices: {1}, baseIndex: {2}, nIndices: {3}, materialIndex: {4}, ",
			//	subMesh.baseVertexIndex, subMesh.nVertices, subMesh.baseIndex, subMesh.nIndices, subMesh.materialIndex);

			VOG_CORE_ASSERT(s_pData->pVertexArray->getVertexBuffer() == meshComponent_.pVertexBuffer, "");
			VOG_CORE_ASSERT(s_pData->pVertexArray->getIndexBuffer() == meshComponent_.pIndexBuffer, "");
			VOG_CORE_ASSERT(s_pData->pVertexArray->getVertexBuffer()->getByteSize() > 0, "");
			VOG_CORE_ASSERT(s_pData->pVertexArray->getIndexBuffer()->getCount() > 0, "");

			pMaterial->uploadMaterial();
			pShader->setMat4("u_transform", transform_);

			if (meshComponent_.isAnimated)
			{
				const auto& boneTransforms = meshComponent_.pModel->getBoneTransforms();
				VOG_CORE_ASSERT(boneTransforms.size() <= s_max_n_bone, "");
				pShader->setMat4Array("u_boneTransformations", boneTransforms.data(), static_cast<uint32_t>(boneTransforms.size()));
			}

			s_pData->pVertexArray->bind();

			RenderCommand::drawIndexBaseVertex(Primitive::Triangle, meshComponent_.pVertexBuffer, meshComponent_.pIndexBuffer, subMesh.baseVertexIndex, subMesh.baseIndex, subMesh.nIndices);
		}
		else
		{
			//VOG_CORE_LOG_WARN("it is not sub mesh, may have bug now!", "");
			pMaterial->uploadMaterial();
			pShader->setMat4("u_transform", transform_);

			s_pData->pVertexArray->bind();
			RenderCommand::drawIndex(Primitive::Triangle, meshComponent_.pVertexBuffer, meshComponent_.pIndexBuffer);
		}
	}

	void Renderer::submitSkybox(const RefPtr<Cubemap>& pCubemap_)		// TODO: remove test
	{
		// TODO: remove test
		VOG_CORE_ASSERT(pCubemap_, "");

		auto pShader = s_pData->shaderLibrary["Skybox"];
		//auto pShader = s_pData->shaderLibrary["Simple_Model_Loading"];

		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);

		pShader->bind();
		pCubemap_->bind();

		pShader->setMat4("u_transform", s_pData->cameraRotationMatrix);

		auto pCubeModel = AssetManager::getMesh(PrimitiveMesh::Box);

		pCubeModel->m_pVertexArray->bind();
		//pCubeModel->m_pVertexArray->setVertexBuffer(pCubeModel->m_pVertexBuffer);
		//pCubeModel->m_pVertexArray->setIndexBuffer(pCubeModel->m_pIndexBuffer);
		RenderCommand::drawIndex(Primitive::Triangle, pCubeModel->m_pVertexBuffer, pCubeModel->m_pIndexBuffer);

		glDepthFunc(GL_LESS);
		glCullFace(GL_BACK);
	}

	ShaderLibrary& Renderer::getShaderLibrary()
	{
		return s_pData->shaderLibrary;
	}

	void Renderer::drawPoints(const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_, const Matrix4f& transform_)
	{
		auto pShader = s_pData->shaderLibrary["vertexPos"];

		pShader->bind();

		pShader->setMat4("u_transform", transform_);

		s_pData->pVertexArray->bind();
		s_pData->pVertexArray->setVertexBuffer(pVertexBuffer_);
		RenderCommand::drawArrays(Primitive::Point, pVertexBuffer_, count_);
	}
}
