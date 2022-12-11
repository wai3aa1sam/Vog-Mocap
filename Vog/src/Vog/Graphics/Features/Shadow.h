#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Graphics/RenderPass/RenderPass.h"
#include "Vog/Graphics/Renderer/Renderer.h"

// References:
// [LearnOpenGL - Shadow Mapping](https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping)

namespace vog {

	class VOG_API Shadow : public NonCopyable
	{
	public:
		Shadow() = default;
		~Shadow() = default;

		void init();
		void upload(const Vector3f& position_, const Vector3f& target_);
		void uploadLightViewMatrix(const Matrix4f& viewMatrix_);

		void process(const std::vector<Submission>* pDeferredSubmissions_, const std::vector<AnimatedSubmission>* pDeferredAnimatedSubmissions_,
			const std::vector<Submission>* pForwardSubmissions_, const std::vector<AnimatedSubmission>* pForwardAnimatedSubmissions_);

		void onImGuiRender();
		const Matrix4f& getLightSpaceMatrix();

		uint32_t getResultID();

	public:
	//private:
		struct ShadowMapping
		{
			ShadowMapping() = default;
			~ShadowMapping() = default;

			void init();
			void upload(const Vector3f& position_, const Vector3f& target_);
			void uploadLightViewMatrix(const Matrix4f& viewMatrix_);

			void process(const std::vector<Submission>* pDeferredSubmissions_, const std::vector<AnimatedSubmission>* pDeferredAnimatedSubmissions_,
							const std::vector<Submission>* pForwardSubmissions_, const std::vector<AnimatedSubmission>* pForwardAnimatedSubmissions_);

			uint32_t getResultID();
			const Matrix4f& getLightSpaceMatrix();

			void onImGuiRender();

			uint32_t size = 2560;
			RenderPass shadowPass;
			Matrix4f lightSpace_matrix;
			float near_plane = 0.1f, far_plane = 7.5f;
			float orthSize = 10.0f;
		};

		ShadowMapping m_shadowMapping;
	};
}