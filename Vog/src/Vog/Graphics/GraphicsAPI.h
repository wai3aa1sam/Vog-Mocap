#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"
#include "Vog/Math/MyMath.h"

#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/GraphicsDefines.h"

#include <type_traits>

namespace vog {

	// Graphics API abstract class
	class VOG_API GraphicsAPI : NonCopyable
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL,
		};

		GraphicsAPI() = default;
		virtual ~GraphicsAPI() = default;

		virtual void init() = 0;
		virtual void shutdown() = 0;

		virtual void clear(BufferBitType bufferBitType_) = 0;
		virtual void setClearColor(const Vector4f& color_) = 0;

		virtual void setViewport(uint32_t width_, uint32_t height_) = 0;
		virtual void setViewport(int x_, int y_, uint32_t width_, uint32_t height_) = 0;

		virtual void drawIndex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t indexCount_ = 0) = 0;
		virtual void drawIndexBaseVertex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t baseVertexIndex_, uint32_t baseIndex_, uint32_t indexCount_) = 0;
		virtual void drawArrays(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_) = 0;

		virtual void setLineWidth(float width_) = 0;

		virtual void setDepthMask(bool enableWriting_) = 0;
		//virtual void setStencilMask(uint32_t value_) = 0;

		virtual void setCullFace(CullFaceType type_) = 0;
		virtual void setDepthFunc(DepthFuncConfig config_) = 0;
		//virtual void setSencilFunc(DepthFuncConfig config_) = 0;

#pragma region Enable_XX
		virtual void setEnableBlending(bool isEnable_) = 0;
		virtual void setEnableCullFace(bool isEnable_) = 0;
		virtual void setEnableDepthTest(bool isEnable_) = 0;
		virtual void setEnableMutliSample(bool isMutliSample_) = 0;
		//virtual void setEnableSentcilTest(bool isEnable_) = 0;
#pragma endregion

		inline virtual API getGraphicsAPI() { return s_api; };
	private:
		static API s_api;
	};
}