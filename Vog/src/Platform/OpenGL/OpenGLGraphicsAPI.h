#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/GraphicsAPI.h"
#include "Vog/Graphics/VertexArray.h"

namespace vog {
	// actual OpenGL API impl for GraphicsAPI abstract class

	class VOG_API OpenGLGraphicsAPI : public GraphicsAPI
	{
	public:
		OpenGLGraphicsAPI() = default;

		virtual ~OpenGLGraphicsAPI();
		virtual void init() override;
		virtual void shutdown() override;

		virtual void clear(BufferBitType bufferBitType_) override;
		virtual void setClearColor(const Vector4f& color_) override;

		virtual void setViewport(uint32_t width_, uint32_t height_) override;
		virtual void setViewport(int x_, int y_, uint32_t width_, uint32_t height_) override;

		virtual void drawIndex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t indexCount_ = 0) override;
		virtual void drawIndexBaseVertex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t baseVertexIndex_, uint32_t baseIndex_, uint32_t indexCount_) override;
		virtual void drawArrays(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_) override;

		virtual void setLineWidth(float width_) override;

		virtual void setDepthMask(bool enableWriting_) override;
		//virtual void setStencilMask(uint32_t value_) override;

		virtual void setCullFace(CullFaceType type_) override;
		virtual void setDepthFunc(DepthFuncConfig config_) override;
		//virtual void setSencilFunc(DepthFuncConfig config_) override;

#pragma region Enable_XX
		virtual void setEnableBlending(bool isEnable_) override;

		virtual void setEnableCullFace(bool isEnable_) override;
		virtual void setEnableMutliSample(bool isMutliSample_) override;
		virtual void setEnableDepthTest(bool isEnable_) override;
		//virtual void setEnableSentcilTest(bool isEnable_) override;
#pragma endregion

		inline virtual API getGraphicsAPI() override { return API::OpenGL; }

	private:
	};
}