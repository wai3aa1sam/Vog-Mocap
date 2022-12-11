#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {
	struct BodyTransform
	{
		Vector3f position = Vector3f{ 0.0f, 0.0f, 0.0f };
		Quaternion orientation = Quaternion{ 1.0f, 0.0f, 0.0f, 0.0f };

		Matrix4f getTransform() { return MyMath::translate(position) * MyMath::toMatrix4f(orientation); }
		Matrix4f getTransform() const { return MyMath::translate(position) * MyMath::toMatrix4f(orientation); }
	};
}

