#include "vogpch.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#include "Platform/OpenGL/OpenGLDefines.h"

#include <glad/glad.h>

namespace vog {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t sizeOfByte_, uint32_t binding_)
		:
		m_sizeOfByte(sizeOfByte_)
	{
		glDebugCall(glCreateBuffers(1, &m_rendererID));
		glDebugCall(glNamedBufferData(m_rendererID, sizeOfByte_, nullptr, GL_DYNAMIC_DRAW));
		// define the range of the buffer that links to a uniform binding point
		glDebugCall(glBindBufferRange(GL_UNIFORM_BUFFER, binding_, m_rendererID, 0, sizeOfByte_));
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		VOG_CORE_ASSERT(m_rendererID, "");
		glDebugCall(glDeleteBuffers(1, &m_rendererID));
	}

	void OpenGLUniformBuffer::setData(const void* pData_, uint32_t sizeOfByte_, uint32_t offset_)
	{
		VOG_CORE_ASSERT(m_rendererID, "");
		VOG_CORE_ASSERT(offset_ + sizeOfByte_ <= m_sizeOfByte, "");

		glDebugCall(glNamedBufferSubData(m_rendererID, offset_, sizeOfByte_, pData_));
	}
}