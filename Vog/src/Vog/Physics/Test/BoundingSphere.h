#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {

	class VOG_API BoundingSphere : public NonCopyable
	{
	public:

	private:
		Vector3f m_center;
		float radius = 0;
	};
}