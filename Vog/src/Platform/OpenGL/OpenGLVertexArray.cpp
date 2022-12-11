#include "vogpch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/OpenGL/OpenGLDefines.h"
#include "Platform/OpenGL/OpenGLUtils.h"

#include "Vog/Graphics/VertexLayout.h"

namespace vog {

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glDebugCall(glGenVertexArrays(1, &m_rendererId));
		glDebugCall(glBindVertexArray(m_rendererId));

		glDebugCall(glBindVertexArray(0));		// prevent other vertex buffer bind then the above will gg

		//VOG_CORE_LOG_INFO("Vertex array created defult: id: {0}", m_rendererId);
	}

	OpenGLVertexArray::OpenGLVertexArray(const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_)
		:
		m_pVertexBuffer(pVertexBuffer_), m_pIndexBuffer(pIndexBuffer_)
	{
		glDebugCall(glGenVertexArrays(1, &m_rendererId));
		glDebugCall(glBindVertexArray(m_rendererId));

		m_pVertexBuffer->bind();
		bindVertexAttributes(m_pVertexBuffer->getVertexLayout());
		m_pIndexBuffer->bind();

		glDebugCall(glBindVertexArray(0));		// prevent other vertex buffer bind then the above will gg

		//VOG_CORE_LOG_INFO("Vertex array created: id: {0}", m_rendererId);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLVertexArray delete Failed, no valid rendererId");
		glDebugCall(glDeleteVertexArrays(1, &m_rendererId));
		m_rendererId = 0;

		//VOG_CORE_LOG_INFO("Vertex array destroyed: id: {0}", m_rendererId);
	}

	void OpenGLVertexArray::bind()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLVertexArray bind Failed, no valid rendererId");

		glDebugCall(glBindVertexArray(m_rendererId));
		//m_pVertexBuffer->bind();
		//if (m_pIndexBuffer)
		//	m_pIndexBuffer->bind();
		//VOG_CORE_LOG_INFO("Vertex array binded: id: {0}", m_rendererId);
	}

	void OpenGLVertexArray::unbind()
	{
		glDebugCall(glBindVertexArray(0));
	}

	inline void OpenGLVertexArray::setVertexBuffer(const RefPtr<VertexBuffer>& pVertexBuffer_)
	{
		glDebugCall(glBindVertexArray(m_rendererId));
		m_pVertexBuffer = pVertexBuffer_;
		m_pVertexBuffer->bind();
		bindVertexAttributes(m_pVertexBuffer->getVertexLayout());
	}

	inline void OpenGLVertexArray::setIndexBuffer(const RefPtr<IndexBuffer>& pIndexBuffer_)
	{
		glDebugCall(glBindVertexArray(m_rendererId));
		m_pIndexBuffer = pIndexBuffer_;
		m_pIndexBuffer->bind();
	}

	void OpenGLVertexArray::bindVertexAttributes(const VertexLayout& layout_)
	{
		uint32_t index = 0;
		size_t offset = 0;

		uint32_t stride = layout_.getTotalElementsBytes();

		for(auto& element : layout_)
		{
			GLenum glslBaseDataType = Utils::toOpenGLBaseDataType(element.type);

			glDebugCall(glEnableVertexAttribArray(index));
			if (glslBaseDataType == GL_INT)
			{
				glDebugCall(glVertexAttribIPointer(index, element.count, glslBaseDataType,
					stride, reinterpret_cast<const void*>((intptr_t)offset)));
			}
			else
			{
				glDebugCall(glVertexAttribPointer(index, element.count, glslBaseDataType,
					element.isNormalized ? GL_TRUE : GL_FALSE, stride, reinterpret_cast<const void*>((intptr_t)offset)));
			}

			offset += Utils::getVertexDataTypeSize(element.type);
			index++;
		}
	}
}