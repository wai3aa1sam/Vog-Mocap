#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Buffer.h"
#include "Vog/Graphics/VertexArray.h"

namespace vog {

	class VOG_API OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		OpenGLVertexArray(const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_);
		virtual ~OpenGLVertexArray();

		virtual void bindVertexAttributes(const VertexLayout& layout_) override;

		virtual void bind() override;
		virtual void unbind() override;

		inline virtual void setVertexBuffer(const RefPtr<VertexBuffer>& pVertexBuffer_) override;
		inline virtual void setIndexBuffer(const RefPtr<IndexBuffer>& pIndexBuffer_) override;

		inline virtual const RefPtr<VertexBuffer>& getVertexBuffer() const override { return m_pVertexBuffer; };
		inline virtual const RefPtr<IndexBuffer>& getIndexBuffer() const override { return m_pIndexBuffer; };

	private:

	private:
		uint32_t m_rendererId = 0;
		RefPtr<VertexBuffer> m_pVertexBuffer;
		RefPtr<IndexBuffer> m_pIndexBuffer;
	};
}