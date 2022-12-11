#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Buffer.h"

namespace vog {

	class VOG_API VertexArray : public NonCopyable
	{
	public:
		VertexArray() = default;
		virtual ~VertexArray() = default;

		virtual void bindVertexAttributes(const VertexLayout& layout_) = 0;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		inline virtual void setVertexBuffer(const RefPtr<VertexBuffer>& pVertexBuffer_) = 0;
		inline virtual void setIndexBuffer(const RefPtr<IndexBuffer>& pIndexBuffer_) = 0;

		inline virtual const RefPtr<VertexBuffer>& getVertexBuffer() const = 0;
		inline virtual const RefPtr<IndexBuffer>& getIndexBuffer() const = 0;

		static RefPtr<VertexArray> create();
		static RefPtr<VertexArray> create(const RefPtr<VertexBuffer>& pVertexBuffer_, const RefPtr<IndexBuffer>& pIndexBuffer_);
	private:
	};
}