#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Buffer.h"

namespace vog {

	class VOG_API OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t totalByteSize_);
		OpenGLVertexBuffer(const void* vertices_, uint32_t totalByteSize_);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() override;
		virtual void unbind() override;

		virtual void setData(const void* data_, uint32_t totalByteSize_) override;

		virtual uint32_t getByteSize() override { return m_byteSize; }

		virtual void setVertexLayout(const VertexLayout& layout_) override;
		inline virtual const VertexLayout& getVertexLayout() const override { return m_vertexLayout; };

	private:
		uint32_t m_rendererId = 0;
		uint32_t m_byteSize;
		VertexLayout m_vertexLayout;
	};

	class VOG_API OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices_, uint32_t count_);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() override;
		virtual void unbind() override;

		inline virtual uint32_t getCount() const override { return m_count; }

	private:
		uint32_t m_rendererId = 0;
		uint32_t m_count = 0;
	};
}