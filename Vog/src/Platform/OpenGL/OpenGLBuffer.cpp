#include "vogpch.h"
#include "OpenGLBuffer.h"
#include "OpenGLDefines.h"

namespace vog {

#pragma region OpenGLVertexBuffer

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t totalByteSize_)
	{
		glDebugCall(glCreateBuffers(1, &m_rendererId));
		glDebugCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
		glDebugCall(glBufferData(GL_ARRAY_BUFFER, totalByteSize_, nullptr, GL_DYNAMIC_DRAW));		// GL_STATIC_DRAW, GL_DYNAMIC_DRAW
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* vertices_, uint32_t totalByteSize_)
	{
		glDebugCall(glCreateBuffers(1, &m_rendererId));
		glDebugCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
		glDebugCall(glBufferData(GL_ARRAY_BUFFER, totalByteSize_, vertices_, GL_DYNAMIC_DRAW));		// GL_STATIC_DRAW, GL_DYNAMIC_DRAW
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLVertexBuffer delete Failed, no valid rendererId");
		glDebugCall(glDeleteBuffers(1, &m_rendererId));
		m_rendererId = 0;
	}

	void OpenGLVertexBuffer::bind()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLVertexBuffer bind Failed, no valid rendererId");
		glDebugCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
	}

	void OpenGLVertexBuffer::unbind()
	{
		glDebugCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void OpenGLVertexBuffer::setData(const void* data_, uint32_t totalByteSize_)
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLVertexBuffer setData Failed, no valid rendererId");
		glDebugCall(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
		glDebugCall(glBufferSubData(GL_ARRAY_BUFFER, 0, totalByteSize_, data_));
	}

	void OpenGLVertexBuffer::setVertexLayout(const VertexLayout& layout_)
	{
		m_vertexLayout = layout_;
	}

#pragma endregion

#pragma region OpenGLIndexBuffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t* indices_, uint32_t count_)
		:
		m_count(count_)
	{
		glDebugCall(glCreateBuffers(1, &m_rendererId));
		glDebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId));
		glDebugCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(uint32_t), indices_, GL_STATIC_DRAW));
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLIndexBuffer delete Failed, no valid rendererId");
		glDebugCall(glDeleteBuffers(1, &m_rendererId));
		m_rendererId = 0;
	}

	void OpenGLIndexBuffer::bind()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLIndexBuffer bind Failed, no valid rendererId");
		glDebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId));
	}

	void OpenGLIndexBuffer::unbind()
	{
		glDebugCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
#pragma endregion

}