#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/RenderCommand.h"
#include "Vog/Graphics/Mesh.h"
#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/Texture.h"
#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/UniformBuffer.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Objects/Camera.h"

#include "Vog/Scene/Components.h"

namespace vog {

#define QUAD_TEMP

	struct Submission
	{
		RefPtr<VertexBuffer> pVertexBuffer;
		RefPtr<IndexBuffer> pIndexBuffer;
		const Matrix4f* pTransform = nullptr;
		RefPtr<Material> pMaterial;

		uint32_t indexCount = 0;

		SubMesh subMesh;
	};

	struct AnimatedSubmission
	{
		RefPtr<VertexBuffer> pVertexBuffer;
		RefPtr<IndexBuffer> pIndexBuffer;
		const Matrix4f* pTransform = nullptr;
		RefPtr<Material> pMaterial;

		uint32_t indexCount = 0;

		SubMesh subMesh;
		const std::vector<Matrix4f>* pBoneTransforms = nullptr;
	};

	class VOG_API Renderer : public NonCopyable
	{
	public:
		static void init(RefPtr<UniformBuffer>& pCameraUniformBuffer_);
		static void shuntdown();

		static void onWindowResize(uint32_t width_, uint32_t height_);

		static void beginScene(const Camera& camera_);
		static void endScene();

		static void submit(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void submit(const RefPtr<Model>& pModel_, float dt_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void submitLines(const RefPtr<Shader>& pShader_, const RefPtr<VertexArray>& pVertexArray_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void submitMesh(const MeshComponent& meshComponent_, const MeshRendererComponent& meshRendererComponent_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static void submitSkybox(const RefPtr<Cubemap>& pCubemap_);		// TODO: remove test

		static void drawPoints(const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_, const Matrix4f& transform_ = Matrix4f(1.0f));

		static ShaderLibrary& getShaderLibrary();

		inline static GraphicsAPI::API getGraphicsAPI() { return RenderCommand::getGraphicsAPI(); }

		

	private:
		struct RendererData
		{
		public:
			bool isInitialized = false;
			RefPtr<UniformBuffer> pCameraUniformBuffer;
			Matrix4f cameraRotationMatrix = Matrix4f(1.0f);

			ShaderLibrary shaderLibrary;

			RefPtr<VertexArray> pVertexArray;

			RefPtr<Shader> pCurrentShader;

		public:
			RendererData();
			~RendererData();
			void init(RefPtr<UniformBuffer>& pCameraUniformBuffer_);
			void destroy();
		};
		static RendererData* s_pData;
	};
}