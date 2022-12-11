#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {

	struct CameraUniformBuffer
	{
		Matrix4f u_view;
		Matrix4f u_projection;
		Matrix4f u_viewProjection;

		Matrix4f u_lightSpace_matrix;

		static RefPtr<UniformBuffer> createUniformBuffer(uint32_t binding_ = 0) { return UniformBuffer::create(sizeof(CameraUniformBuffer), binding_); }
	};
}