#include "vogpch.h"
#include "Platform/OpenGL/OpenGLGraphicsAPI.h"
#include "Platform/OpenGL/OpenGLDefines.h"

#include "Platform/OpenGL/OpenGLUtils.h"

#include <type_traits>

namespace vog {

	void OpenGLGraphicsAPI::clear(BufferBitType bufferBitType_)
	{
		// TODO: here only convert one but flag, | will have errors
		glClear(Utils::bitConcatOpenGLBufferBitType(bufferBitType_));
	}

	OpenGLGraphicsAPI::~OpenGLGraphicsAPI()
	{
	}

	void OpenGLGraphicsAPI::init()
	{
		glDebugCall(glEnable(GL_DEPTH_TEST));
		glDebugCall(glDepthMask(GL_TRUE));
		glDebugCall(glEnable(GL_MULTISAMPLE));
		glDebugCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
		//glDebugCall(glEnable(GL_BLEND));
		//glDebugCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		glDebugCall(glEnable(GL_LINE_SMOOTH));

		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(1.0, 1.0);
	}

	void OpenGLGraphicsAPI::shutdown()
	{
	}

	void OpenGLGraphicsAPI::setClearColor(const Vector4f& color_)
	{
		glDebugCall(glClearColor(color_.r, color_.g, color_.b, color_.a));
	}

	void OpenGLGraphicsAPI::setViewport(uint32_t width_, uint32_t height_)
	{
		glDebugCall(glViewport(0, 0, width_, height_));
	}

	void OpenGLGraphicsAPI::setViewport(int x_, int y_, uint32_t width_, uint32_t height_)
	{
		glDebugCall(glViewport(x_, y_, width_, height_));
	}

	void OpenGLGraphicsAPI::drawIndex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t indexCount_)
	{
		uint32_t indexCount = indexCount_ ? indexCount_ : pIndexBuffer_->getCount();
		glDebugCall(glDrawElements(Utils::toOpenGLPrimitive(primitive_), indexCount, GL_UNSIGNED_INT, 0));
	}

	void OpenGLGraphicsAPI::drawIndexBaseVertex(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_, uint32_t baseVertexIndex_, uint32_t baseIndex_, uint32_t indexCount_)
	{
		glDebugCall(glDrawElementsBaseVertex(Utils::toOpenGLPrimitive(primitive_), indexCount_, GL_UNSIGNED_INT, reinterpret_cast<void*>(sizeof(uint32_t) * baseIndex_), baseVertexIndex_));
	}

	void OpenGLGraphicsAPI::drawArrays(Primitive primitive_, const RefPtr<VertexBuffer>& pVertexBuffer_, uint32_t count_)
	{
		glDebugCall(glDrawArrays(Utils::toOpenGLPrimitive(primitive_), 0, count_));
	}

	void OpenGLGraphicsAPI::setLineWidth(float width_)
	{
		glDebugCall(glLineWidth(width_));
	}

	void OpenGLGraphicsAPI::setDepthMask(bool enableWriting_)
	{
		glDebugCall(glDepthMask((GLboolean)enableWriting_));
	}

	void OpenGLGraphicsAPI::setCullFace(CullFaceType type_)
	{
		glDebugCall(glCullFace(Utils::toOpenGLCullFaceType(type_)));
	}

	void OpenGLGraphicsAPI::setDepthFunc(DepthFuncConfig config_)
	{
		glDebugCall(glDepthFunc(Utils::toOpenGLDepthFuncConfig(config_)));
	}

	void OpenGLGraphicsAPI::setEnableBlending(bool isEnable_)
	{
		if (isEnable_)
		{
			glDebugCall(glEnable(GL_BLEND));
			glDebugCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		}
		else
		{
			glDebugCall(glDisable(GL_BLEND));
		}
	}

	void OpenGLGraphicsAPI::setEnableCullFace(bool isEnable_)
	{
		if (isEnable_)
		{
			glDebugCall(glEnable(GL_CULL_FACE));
		}
		else
		{
			glDebugCall(glDisable(GL_CULL_FACE));
		}
	}

	void OpenGLGraphicsAPI::setEnableMutliSample(bool isMutliSample_)
	{
		if (isMutliSample_)
		{
			glDebugCall(glEnable(GL_MULTISAMPLE));
		}
		else
		{
			glDebugCall(glDisable(GL_MULTISAMPLE));
		}
	}
	void OpenGLGraphicsAPI::setEnableDepthTest(bool isEnable_)
	{
		if (isEnable_)
		{
			glDebugCall(glEnable(GL_DEPTH_TEST));
		}
		else
		{
			glDebugCall(glDisable(GL_DEPTH_TEST));
		}
	}
}
