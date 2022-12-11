#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/RenderCommand.h"
#include "Vog/Graphics/Mesh.h"
#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/Texture.h"
#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/UniformBuffer.h"

#include "Vog/Graphics/RenderPass/RenderPass.h"
#include "Vog/Graphics/Renderer/Renderer.h"

#include "Vog/Graphics/Features/Bloom.h"
#include "Vog/Graphics/Features/Shadow.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Objects/Camera.h"

namespace vog {

	class VOG_API DeferredRenderer : public NonCopyable
	{
	public:
		
	public:
		static void init(RefPtr<UniformBuffer>& pCameraUniformBuffer_);
		static void shuntdown();

		static void onWindowResize(uint32_t width_, uint32_t height_);

		static void beginScene(const Camera& camera_);
		static void endScene();

		static uint32_t getReusltID(uint32_t index_ = 0);

		static void uploadLights(void* positions_, void* directions_, void* colors_, void* params_);
		static void uploadLighViewMatrix(const Matrix4f& lightViewMatrix_);

		static void submit(Submission&& submission_);
		static void submit(AnimatedSubmission&& submission_);

		static void submit(const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, const Matrix4f& transform_, const RefPtr<Material>& pMaterial_ = nullptr);

		//static void submit(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_ = Matrix4f(1.0f));

		//static void submit(const RefPtr<Model>& pModel_, float dt_, const Matrix4f& transform_ = Matrix4f(1.0f));

		//static void submitLines(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_ = Matrix4f(1.0f));

		//static void submitMesh(const MeshComponent& meshComponent_, const MeshRendererComponent& meshRendererComponent_, const Matrix4f& transform_ = Matrix4f(1.0f));

		//static void drawPoints(const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void onImGuiRender();

		static std::pair<float, float> getViewportSize();

		inline static GraphicsAPI::API getGraphicsAPI() { return RenderCommand::getGraphicsAPI(); }

		// TODO: remove
		static uint32_t temp_getShadowmap() { return s_pData->shadow.m_shadowMapping.shadowPass.getFramebuffer()->getDepthAttachmentID(); }
		//static void temp_setShadowEntity(Entity entity_) { s_pData->light = entity_; }

	private:
		static void _flush();

	public:
		struct RendererData
		{
		public:
			bool isInitialized = false;
			RefPtr<UniformBuffer> pCameraUniformBuffer;
			Matrix4f cameraRotationMatrix = Matrix4f(1.0f);
			Vector3f cameraPosition = { 0.0f, 0.0f, 0.0f };

			RenderPass geometryPass;
			//RenderPass lightingPass;		// light pass framebuffer use postprocess's framebuffer
			RenderPass postprocessingPass;

			RefPtr<Material> pDeferred_postprocess_material;

			RefPtr<VertexArray> pVertexArray;

			std::vector<Submission> deferredSubmissions;
			std::vector<AnimatedSubmission> deferredAnimatedSubmissions;

			std::vector<Submission> forwardSubmissions;
			std::vector<AnimatedSubmission> forwardAnimatedSubmissions;

			Bloom bloom;
			Shadow shadow;

			Vector4f screenParam = { 0.0f, 0.0f, 0.0f, 0.0f };

			void* pLight_positions	= nullptr;
			void* pLight_directions = nullptr;
			void* pLight_colors		= nullptr;
			void* pLight_params		= nullptr;

		public:
			RendererData();
			~RendererData();
			void init(RefPtr<UniformBuffer>& pCameraUniformBuffer_);
			void destroy();
		};
		static RendererData* s_pData;
	};
}