#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/RenderPass/RenderPass.h"

// References:
// https://learnopengl.com/Advanced-Lighting/Bloom

namespace vog {

	class VOG_API Bloom : public NonCopyable
	{
	public:
		Bloom();
		~Bloom();

		void init();

		uint32_t processBlur(uint32_t textureID_);

		int getIteration();
		void setIteration(int iteration_);

	private:
		struct PingpongPass
		{
			RefPtr<Framebuffer> pPingpongFB[2];
			RefPtr<Shader> pShader;
			RefPtr<VertexArray> pVertexArray;

			void init();
			uint32_t processBlur(uint32_t textureID_, int iteration_ = 10);
		};

		int m_iteration = 10;
		PingpongPass m_pingpongPass;
	};

	inline int Bloom::getIteration() { return m_iteration; }
	inline void Bloom::setIteration(int iteration_) { m_iteration = iteration_; }
}