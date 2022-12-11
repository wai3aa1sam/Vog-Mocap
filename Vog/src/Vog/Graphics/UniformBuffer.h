#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog {

	class VOG_API UniformBuffer : public NonCopyable
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;

		virtual void setData(const void* pData_, uint32_t sizeOfByte_, uint32_t offset_) = 0;

		static RefPtr<UniformBuffer> create(uint32_t sizeOfByte_, uint32_t binding_);

	private:
	};
}