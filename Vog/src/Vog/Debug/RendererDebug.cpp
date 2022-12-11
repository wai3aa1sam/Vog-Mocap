#include "vogpch.h"
#include "Vog/Debug/RendererDebug.h"

#include "Vog/Graphics/Renderer/RendererInitData.h"

#include "Vog/Resources/AssetManager.h"

#include <imgui.h>

namespace vog {

	RendererDebug::RendererData* RendererDebug::s_pData = new RendererDebug::RendererData;

#pragma region RendererData
	RendererDebug::RendererData::CircleData::~CircleData()
	{
		destroy();
	}
	void RendererDebug::RendererData::CircleData::init(const RefPtr<IndexBuffer>& pIndexBuffer_)
	{
		pCircleVertexArray = VertexArray::create();

		pCircleVertexBuffer = VertexBuffer::create(maxQuadVertices * sizeof(CircleVertex));
		pCircleVertexBuffer->setVertexLayout({
			{ VertexDataType::Float3, "a_worldPosition" },
			{ VertexDataType::Float3, "a_uv" },
			{ VertexDataType::Float4, "a_color"         },
			{ VertexDataType::Float,  "a_thickness"     },
			{ VertexDataType::Float,  "a_fade"          },
			});
		pCircleVertexArray->setVertexBuffer(pCircleVertexBuffer);
		pCircleVertexArray->setIndexBuffer(pIndexBuffer_); // Use squareIB
		pIndexBuffer = pIndexBuffer_;
		pCircleVertexBufferBase = new CircleVertex[maxQuadVertices];
		itCircleVertexBufferBase = pCircleVertexBufferBase;
		circleIndicesCount = 0;

		pCircleVertexArray->unbind();
	}
	void RendererDebug::RendererData::CircleData::destroy()
	{
		delete[] pCircleVertexBufferBase;
		pCircleVertexBufferBase = nullptr;
		itCircleVertexBufferBase = nullptr;
		pCircleVertexArray.reset();
		pCircleVertexBuffer.reset();
	}

	RendererDebug::RendererData::LineData::~LineData()
	{
		destroy();
	}

	void RendererDebug::RendererData::LineData::init()
	{
		pLineVertexArray = VertexArray::create();

		pLineVertexBuffer = VertexBuffer::create(maxQuadVertices * sizeof(LineVertex));
		pLineVertexBuffer->setVertexLayout({
			{ VertexDataType::Float3, "a_worldPosition" },
			{ VertexDataType::Float4, "a_color"    },
			});
		pLineVertexArray->setVertexBuffer(pLineVertexBuffer);
		pLineVertexBufferBase = new LineVertex[maxQuadVertices];
		itLineVertexBufferBase = pLineVertexBufferBase;
	}

	void RendererDebug::RendererData::LineData::destroy()
	{
		delete[] pLineVertexBufferBase;
		pLineVertexBufferBase = nullptr;
		itLineVertexBufferBase = nullptr;
		pLineVertexArray.reset();
		pLineVertexBuffer.reset();
	}

	RendererDebug::RendererData::RendererData()
	{
		isInitialized = true;
	}

	RendererDebug::RendererData::~RendererData()
	{
		destroy();
	}

	void RendererDebug::RendererData::init(RefPtr<UniformBuffer>& pCameraUniformBuffer_)
	{
		VOG_CORE_ASSERT(isInitialized, "static order problem!");

		pCameraUniformBuffer = pCameraUniformBuffer_;

		pVertexArray = VertexArray::create();
#pragma region Circle_and_line_init
		// indices buffer init
		uint32_t* quadIndices = new uint32_t[maxQuadIndices];
		uint32_t offset = 0;

		//uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		for (uint32_t i = 0; i < maxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		RefPtr<IndexBuffer> pIndexBuffer;
		pIndexBuffer = IndexBuffer::create(quadIndices, maxQuadIndices);
		delete[] quadIndices;		// assume upload to gpu immediately

		// Circle
		circleData.init(pIndexBuffer);
		// Circle end

		// Line
		lineData.init();
		// Line end
#pragma endregion

		FramebufferSpecification specification;
		specification.width = 1280;
		specification.height = 720;
		specification.samples = 1;
		auto pVA = VertexArray::create();
		specification.attachment = { FramebufferTextureFormat::RGBA16F, FramebufferTextureFormat::Depth };
		postprocessingPass.init(Framebuffer::create(specification), AssetManager::getShader("Debug_Postprocess"), pVA);

		//pDebug_Postprocess_material = Material::create(postprocessingPass.getShader());
	}

	void RendererDebug::RendererData::destroy()
	{
		VOG_CORE_ASSERT(isInitialized, "static order problem!");

		pCameraUniformBuffer.reset();

		pCurrentShader.reset();
		pVertexArray.reset();

		circleData.destroy();
		lineData.destroy();

		isInitialized = false;
	}
#pragma endregion

	void RendererDebug::init(RefPtr<UniformBuffer>& pCameraUniformBuffer_)
	{
		if (s_pData)
		{
			if (s_pData->isInitialized)
			{
				s_pData->init(pCameraUniformBuffer_);
			}
		}
	}

	void RendererDebug::shuntdown()
	{
		delete s_pData;
	}

	void RendererDebug::onWindowResize(uint32_t width_, uint32_t height_)
	{
		s_pData->screenParam.x = static_cast<float>(width_);
		s_pData->screenParam.y = static_cast<float>(height_);

		s_pData->postprocessingPass.onWindowResize(width_, height_);
	}

	void RendererDebug::beginScene(const Camera& camera_)
	{
		CameraUniformBuffer cameraUniformBuffer;
		cameraUniformBuffer.u_view = camera_.getViewMatrix();
		cameraUniformBuffer.u_projection = camera_.getProjectionMatrix();
		cameraUniformBuffer.u_viewProjection = camera_.getViewProjectionMatrix();
		s_pData->pCameraUniformBuffer->setData(&cameraUniformBuffer, sizeof(cameraUniformBuffer), 0);
		s_pData->cameraRotationMatrix = camera_.getViewRotationMatrix();

		RenderCommand::setViewport((uint32_t)s_pData->screenParam.x, (uint32_t)s_pData->screenParam.y);

		_startBatch();
	}

	void RendererDebug::endScene()
	{
		_flush();
	}

	uint32_t RendererDebug::getReusltID(uint32_t index_) { return s_pData->postprocessingPass.getResultID(index_); }
	
	void RendererDebug::setScreenMap(uint32_t textureID_) { s_pData->screenMap_textureID = textureID_; }

	void RendererDebug::submitLines(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_)
	{
		pShader_->bind();
		pShader_->setMat4("u_transform", transform_);

		pVertexArray_->bind();
		RenderCommand::drawIndex(Primitive::Line, pVertexArray_->getVertexBuffer(), pVertexArray_->getIndexBuffer());
	}

	void RendererDebug::_startBatch()
	{
		s_pData->circleData.circleIndicesCount = 0;
		s_pData->circleData.itCircleVertexBufferBase = s_pData->circleData.pCircleVertexBufferBase;

		s_pData->lineData.lineVertexCount = 0;
		s_pData->lineData.itLineVertexBufferBase = s_pData->lineData.pLineVertexBufferBase;
	}

	void RendererDebug::_flush()
	{
		s_pData->postprocessingPass.bind();

		RenderCommand::setEnableCullFace(false);
		RenderCommand::setEnableDepthTest(false);
		
		{	// draw screen map
			RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
			RenderCommand::clear(BufferBitType::Color | BufferBitType::Depth);
			RenderCommand::setEnableDepthTest(false);

			auto& pShader = s_pData->postprocessingPass.getShader();
			pShader->bind();

			//{	// for develop purpose
			//	s_pData->pDebug_Postprocess_material->setFloat4("u_screenParam", s_pData->screenParam);
			//	s_pData->pDebug_Postprocess_material->uploadMaterial();
			//}

			uint32_t texture_binding = pShader->getBindingLocation("u_screen_map");
			auto screen_texture_id = s_pData->screenMap_textureID;
			Texture2D::bind(screen_texture_id, texture_binding);
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

		if (s_pData->circleData.circleIndicesCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_pData->circleData.itCircleVertexBufferBase - (uint8_t*)s_pData->circleData.pCircleVertexBufferBase);
			s_pData->circleData.pCircleVertexBuffer->setData(s_pData->circleData.pCircleVertexBufferBase, dataSize);

			AssetManager::getShader("Debug_Circle")->bind();
			s_pData->circleData.pCircleVertexArray->bind();
			RenderCommand::drawIndex(Primitive::Triangle, s_pData->circleData.pCircleVertexBuffer, s_pData->circleData.pIndexBuffer, s_pData->circleData.circleIndicesCount);
		}

		if (s_pData->lineData.lineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)s_pData->lineData.itLineVertexBufferBase - (uint8_t*)s_pData->lineData.pLineVertexBufferBase);
			s_pData->lineData.pLineVertexBuffer->setData(s_pData->lineData.pLineVertexBufferBase, dataSize);

			AssetManager::getShader("Debug_Line")->bind();
			//RenderCommand::setLineWidth(s_pData->lineWidth);
			s_pData->lineData.pLineVertexArray->bind();
			RenderCommand::drawArrays(Primitive::Line, s_pData->lineData.pLineVertexArray->getVertexBuffer(), s_pData->lineData.lineVertexCount);
		}

		s_pData->postprocessingPass.getFramebuffer()->unbind();
	}

	void RendererDebug::_flushAndReset()
	{
		_flush();
		_startBatch();
	}

	void RendererDebug::drawDebugSphere(const Matrix4f& transform_, const Vector4f& color_, float thickness_, float fade_)
	{
		if (s_pData->circleData.circleIndicesCount >= maxQuadIndices)
			_flushAndReset();

		std::array<Matrix4f, 3> rotations = {
			Matrix4f(1.0f),
			MyMath::rotate(MyMath::radians(90.0f), { 1.0f, 0.0f, 0.0f }),
			MyMath::rotate(MyMath::radians(90.0f), { 0.0f, 1.0f, 0.0f }),
		};

		for (int i = 0; i < rotations.size(); i++)
		{
			for (int j = 0; j < 4; j++)
			{
				s_pData->circleData.itCircleVertexBufferBase->worldPosition = transform_ * rotations[i] * s_pData->quadVertexPositions[j];
				s_pData->circleData.itCircleVertexBufferBase->uv = s_pData->quadVertexPositions[j] * 2.0f;
				s_pData->circleData.itCircleVertexBufferBase->color = color_;
				s_pData->circleData.itCircleVertexBufferBase->thickness = thickness_;
				s_pData->circleData.itCircleVertexBufferBase->fade = fade_;
				s_pData->circleData.itCircleVertexBufferBase++;
			}
		}

		s_pData->circleData.circleIndicesCount += 6 * static_cast<uint32_t>(rotations.size());
	}

	void RendererDebug::drawLine(const Vector3f& p0_, const Vector3f& p1_, const Vector4f& color_)
	{
		if (s_pData->lineData.lineVertexCount >= maxQuadIndices)
			_flushAndReset();

		s_pData->lineData.itLineVertexBufferBase->worldPosition = p0_;
		s_pData->lineData.itLineVertexBufferBase->color = color_;
		s_pData->lineData.itLineVertexBufferBase++;

		s_pData->lineData.itLineVertexBufferBase->worldPosition = p1_;
		s_pData->lineData.itLineVertexBufferBase->color = color_;
		s_pData->lineData.itLineVertexBufferBase++;

		s_pData->lineData.lineVertexCount += 2;
	}

	void RendererDebug::drawLineBox(const Matrix4f& transform_, const Vector4f& color_)
	{
		Vector3f lineVertices[8];
		for (size_t i = 0; i < 8; i++)
			lineVertices[i] = transform_ * s_pData->boxVertexPositions[i];

		drawLine(lineVertices[0], lineVertices[1], color_);
		drawLine(lineVertices[1], lineVertices[2], color_);
		drawLine(lineVertices[2], lineVertices[3], color_);
		drawLine(lineVertices[3], lineVertices[0], color_);

		drawLine(lineVertices[4], lineVertices[5], color_);
		drawLine(lineVertices[5], lineVertices[6], color_);
		drawLine(lineVertices[6], lineVertices[7], color_);
		drawLine(lineVertices[7], lineVertices[4], color_);

		drawLine(lineVertices[2], lineVertices[6], color_);
		drawLine(lineVertices[1], lineVertices[5], color_);
		drawLine(lineVertices[3], lineVertices[7], color_);
		drawLine(lineVertices[0], lineVertices[4], color_);
	}

	void RendererDebug::drawPlane(const Matrix4f& transform_, const Vector4f& color_)
	{
		Vector3f lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform_ * s_pData->planePositions[i];

		drawLine(lineVertices[0], lineVertices[1], color_);
		drawLine(lineVertices[2], lineVertices[0], color_);

		drawLine(lineVertices[1], lineVertices[2], color_);

		drawLine(lineVertices[1], lineVertices[3], color_);
		drawLine(lineVertices[3], lineVertices[2], color_);
	}

	void RendererDebug::drawTriangle(const Matrix4f& transform_, const Vector4f& color_)
	{
		Vector3f lineVertices[3];
		for (size_t i = 0; i < 3; i++)
			lineVertices[i] = transform_ * s_pData->planePositions[i];

		drawLine(lineVertices[0], lineVertices[1], color_);
		drawLine(lineVertices[2], lineVertices[0], color_);
		drawLine(lineVertices[1], lineVertices[2], color_);
	}

	void RendererDebug::drawPoints(const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_, const Matrix4f& transform_)
	{
		auto pShader = AssetManager::getShader("vertexPos");

		pShader->bind();

		pShader->setMat4("u_transform", transform_);

		s_pData->pVertexArray->bind();
		s_pData->pVertexArray->setVertexBuffer(pVertexBuffer_);
		RenderCommand::drawArrays(Primitive::Point, pVertexBuffer_, count_);
	}

	void RendererDebug::drawAxis_XYZ(const Matrix4f& transform_)
	{
		Vector3f lineVertices[] =
		{
			Vector3f(0.0f, 0.0f, 0.0f),
			Vector3f(1.0f, 0.0f, 0.0f),
			Vector3f(0.0f, 1.0f, 0.0f),
			Vector3f(0.0f, 0.0f, 1.0f),
		};
		Vector4f red	= { 1.0f, 0.0f, 0.0f, 1.0f };
		Vector4f green	= { 0.0f, 1.0f, 0.0f, 1.0f };
		Vector4f blue	= { 0.0f, 0.0f, 1.0f, 1.0f };

		drawLine(lineVertices[0], lineVertices[1], red);
		drawLine(lineVertices[0], lineVertices[2], green);
		drawLine(lineVertices[0], lineVertices[3], blue);
	}

	void RendererDebug::onImGuiRender()
	{
		/*ImGui::Text(s_pData->pDebug_Postprocess_material->getShader()->getName().c_str());
		s_pData->pDebug_Postprocess_material->onImGuiRender();*/
	}

}
