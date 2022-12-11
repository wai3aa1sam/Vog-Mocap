#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/GraphicsAPI.h"

namespace vog {

	class VOG_API RenderCommand : public NonCopyable
	{
	public:

		inline static void init(){ s_pInstance->init(); }

		inline static void shutdown(){ s_pInstance->shutdown(); }

		inline static void clear(BufferBitType bufferBitType_) { s_pInstance->clear(bufferBitType_); }
		inline static void setClearColor(const Vector4f& color_ = {0.0f, 0.0f, 0.0f, 0.0f}) { s_pInstance->setClearColor(color_); }


		inline static void setViewport(uint32_t width_, uint32_t height_) { s_pInstance->setViewport(width_, height_); }

		inline static void setViewport(int x_, int y_, uint32_t width_, uint32_t height_) { s_pInstance->setViewport(x_, y_, width_, height_); }

		inline static void drawIndex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t indexCount_ = 0) { s_pInstance->drawIndex(primitive_, pVertexBuffer_, pIndexBuffer_, indexCount_); }
		inline static void drawIndexBaseVertex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t baseVertexIndex_, uint32_t baseIndex_, uint32_t indexCount_) { s_pInstance->drawIndexBaseVertex(primitive_, pVertexBuffer_, pIndexBuffer_, baseVertexIndex_, baseIndex_, indexCount_); };
		inline static void drawArrays(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_) { s_pInstance->drawArrays(primitive_, pVertexBuffer_, count_); };

		inline static void setLineWidth(float width_) { s_pInstance->setLineWidth(width_); }

		inline static void setCullFace(CullFaceType type_) { s_pInstance->setCullFace(type_); }
		inline static void setDepthMask(bool enableWriting_) { s_pInstance->setDepthMask(enableWriting_); }
		inline static void setDepthFunc(DepthFuncConfig config_) { s_pInstance->setDepthFunc(config_); }
		//inline static void setStencilMask(uint32_t value_);
		//inline static void setSencilFunc(DepthFuncConfig config_);

#pragma region Enale_XX
		inline static void setEnableBlending(bool isEnable_) { s_pInstance->setEnableBlending(isEnable_); }

		inline static void setEnableCullFace(bool isEnable_) { s_pInstance->setEnableCullFace(isEnable_); }
		inline static void setEnableDepthTest(bool isEnable_) { s_pInstance->setEnableDepthTest(isEnable_); }
		inline static void setEnableMutliSample(bool isMutliSample_) { s_pInstance->setEnableMutliSample(isMutliSample_); }
#pragma endregion

		inline static GraphicsAPI::API getGraphicsAPI() { return s_pInstance->getGraphicsAPI(); };

	private:
		static ScopePtr<GraphicsAPI> s_pInstance;
	};
}