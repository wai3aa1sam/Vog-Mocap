#pragma once
#include "Vog/Core/Core.h"

namespace vog {

	using UUID_Type = uint64_t;

	class VOG_API UUID
	{
	public:
		UUID();
		UUID(UUID_Type uuid_);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_uuid; }
	private:
		UUID_Type m_uuid;
	};
}