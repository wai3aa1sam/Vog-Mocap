#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/UniformBuffer.h"

namespace vog {

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t sizeOfByte_, uint32_t binding_);
		virtual ~OpenGLUniformBuffer();

		virtual void setData(const void* pData_, uint32_t sizeOfByte_, uint32_t offset_) override;
	private:
		uint32_t m_rendererID = 0;

		uint32_t m_sizeOfByte = 0;
	};
}