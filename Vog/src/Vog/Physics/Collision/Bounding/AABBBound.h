#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {

	class VOG_API AABBBound
	{
	public:
		AABBBound() = default;
		AABBBound(const Vector3f& min_, const Vector3f& max_)
			:
			m_min(min_), m_max(max_)
		{
		}
		void set(const Vector3f& min_, const Vector3f& max_)
		{
			m_min = min_;
			m_max = max_;
		}

		Vector3f getCenter()	{ return (m_max + m_min) * 0.5f; }
		Vector3f getSize()		{ return (m_max - m_min) * 0.5f; }			// half extend
	private:
		Vector3f m_min = { 0.0f, 0.0f, 0.0f };
		Vector3f m_max = { 0.0f, 0.0f, 0.0f };
	};
}