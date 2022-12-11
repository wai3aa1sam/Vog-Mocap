#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Buffer.h"
#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/Shader.h"

#include "Vog/Objects/Camera.h"

namespace vog {

	class VOG_API Gizmos : public NonCopyable
	{
	public:
		static void init();

		static void shuntdown();	// since the member is static, no this then will destroy after Application is deleted;

		static void drawAxis(const Vector3f& position_);

	private:
	};
}