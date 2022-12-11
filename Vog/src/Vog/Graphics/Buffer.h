#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/VertexLayout.h"

#include <vector>

namespace vog {

	class VOG_API VertexBuffer : public NonCopyable
	{
	public:
		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void setData(const void* data_, uint32_t totalByteSize_) = 0;
		virtual uint32_t getByteSize() = 0;

		virtual void setVertexLayout(const VertexLayout& layout_) = 0;
		inline virtual const VertexLayout& getVertexLayout() const = 0;

		static RefPtr<VertexBuffer> create(uint32_t totalByteSize_);
		static RefPtr<VertexBuffer> create(const void* vertices_, uint32_t totalByteSize_);
	private:
	};

	class VOG_API IndexBuffer : public NonCopyable
	{
	public:
		IndexBuffer() = default;
		virtual ~IndexBuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		inline virtual uint32_t getCount() const = 0;

		static RefPtr<IndexBuffer> create(const uint32_t* indices_, uint32_t count_);
	private:
	};
}