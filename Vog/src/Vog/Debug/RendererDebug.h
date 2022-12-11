#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/RenderCommand.h"
#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/Texture.h"
#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/Material.h"
#include "Vog/Graphics/UniformBuffer.h"

#include "Vog/Graphics/RenderPass/RenderPass.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Objects/Camera.h"

namespace vog {

	class VOG_API RendererDebug : public NonCopyable
	{
	public:
		static void init(RefPtr<UniformBuffer>& pCameraUniformBuffer_);
		static void shuntdown();

		static void onWindowResize(uint32_t width_, uint32_t height_);

		static void beginScene(const Camera& camera_);
		static void endScene();

		static void setScreenMap(uint32_t textureID_);
		static uint32_t getReusltID(uint32_t index_ = 0);

		static void submitLines(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void drawDebugSphere(const Matrix4f& transform_, const Vector4f& color_, float thickness_ = 0.01f, float fade_ = 0.005f);
		static void drawLine(const Vector3f& p0_, const Vector3f& p1_, const Vector4f& color_);
		static void drawLineBox(const Matrix4f& transform_, const Vector4f& color_);

		static void drawPlane(const Matrix4f& transform_, const Vector4f& color_);
		static void drawTriangle(const Matrix4f& transform_, const Vector4f& color_);

		static void drawPoints(const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void drawAxis_XYZ(const Matrix4f& transform_ = Matrix4f(1.0f));

		static void onImGuiRender();

		inline static GraphicsAPI::API getGraphicsAPI() { return RenderCommand::getGraphicsAPI(); }

	private:
		static void _startBatch();
		static void _flush();
		static void _flushAndReset();

	private:
		static const uint32_t maxQuad = 100000;
		static const uint32_t maxQuadVertices = maxQuad * 4;
		static const uint32_t maxQuadIndices = maxQuad * 6;

		struct RendererData
		{
			struct CircleData
			{
				struct CircleVertex
				{
					Vector3f worldPosition;
					Vector3f uv;
					Vector4f color;
					float thickness;
					float fade;
				};

			public:
				// Circle
				RefPtr<VertexArray> pCircleVertexArray;
				RefPtr<VertexBuffer> pCircleVertexBuffer;
				RefPtr<IndexBuffer> pIndexBuffer;
				//RefPtr<Shader> pCircleShader;

				uint32_t circleIndicesCount = 0;
				CircleVertex* pCircleVertexBufferBase = nullptr;
				CircleVertex* itCircleVertexBufferBase = nullptr;

			public:
				~CircleData();
				void init(const RefPtr<IndexBuffer>& pIndexBuffer_);
				void destroy();
			};

			struct LineData
			{
				struct LineVertex
				{
					Vector3f worldPosition;
					Vector4f color;
				};

			public:
				// Line
				RefPtr<VertexArray> pLineVertexArray;
				RefPtr<VertexBuffer> pLineVertexBuffer;
				//RefPtr<Shader> pLineShader;

				uint32_t lineVertexCount = 0;
				LineVertex* pLineVertexBufferBase = nullptr;
				LineVertex* itLineVertexBufferBase = nullptr;

			public:
				~LineData();
				void init();
				void destroy();
			};

			bool isInitialized = false;
			Vector4f quadVertexPositions[4] = {
				Vector4f(-0.5f, -0.5f, 0.0f, 1.0f),
				Vector4f(0.5f, -0.5f, 0.0f, 1.0f),
				Vector4f(0.5f,  0.5f, 0.0f, 1.0f),
				Vector4f(-0.5f,  0.5f, 0.0f, 1.0f),
			};

			Vector4f boxVertexPositions[8] = {
				Vector4f(-0.5f, -0.5f, 0.5f, 1.0f),
				Vector4f(0.5f, -0.5f, 0.5f, 1.0f),
				Vector4f(0.5f,  0.5f, 0.5f, 1.0f),
				Vector4f(-0.5f,  0.5f, 0.5f, 1.0f),

				Vector4f(-0.5f, -0.5f, -0.5f, 1.0f),
				Vector4f(0.5f, -0.5f, -0.5f, 1.0f),
				Vector4f(0.5f,  0.5f, -0.5f, 1.0f),
				Vector4f(-0.5f,  0.5f, -0.5f, 1.0f),
			};

			Vector4f planePositions[4] = {
				Vector4f(-0.5f, 0.0f,  0.5f, 1.0f),
				Vector4f(-0.5f, 0.0f, -0.5f, 1.0f),
				Vector4f( 0.5f, 0.0f,  0.5f, 1.0f),
				Vector4f( 0.5f, 0.0f, -0.5f, 1.0f),
			};

			RefPtr<UniformBuffer> pCameraUniformBuffer;
			Matrix4f cameraRotationMatrix = Matrix4f(1.0f);

			RefPtr<VertexArray> pVertexArray;
			RefPtr<Shader> pCurrentShader;

			RenderPass postprocessingPass;
			//RefPtr<Material> pDebug_Postprocess_material;

			CircleData circleData;

			LineData lineData;

			float lineWidth = 2.0f;

			Vector4f screenParam = { 0.0f, 0.0f, 0.0f, 0.0f };

			uint32_t screenMap_textureID = 0;

			RendererData();
			~RendererData();
			void init(RefPtr<UniformBuffer>& pCameraUniformBuffer_);
			void destroy();
		};

		static RendererData* s_pData;
	};
}