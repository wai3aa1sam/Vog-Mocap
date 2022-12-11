#include "vogpch.h"
#include "Gizmos.h"

#include "Vog/Debug/RendererDebug.h"

namespace vog {

	struct GizmosData
	{
		RefPtr<Shader> m_pShader;
		RefPtr<VertexArray> m_pVA;
	};

	static GizmosData s_data;

	void Gizmos::init()
	{
		s_data.m_pShader = Shader::create("assets/shaders/vertexPosColor.glsl");

		struct vertexLayout
		{
			Vector3f position;
			Vector4f color;
		};

		// axis
		vertexLayout vertices[] =
		{
			{ Vector3f(0.0f, 0.0f, 0.0f), Vector4f(1.0f, 0.0f, 0.0f, 1.0f) },
			{ Vector3f(1.0f, 0.0f, 0.0f), Vector4f(1.0f, 0.0f, 0.0f, 1.0f) },

			{ Vector3f(0.0f, 0.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },
			{ Vector3f(0.0f, 1.0f, 0.0f), Vector4f(0.0f, 1.0f, 0.0f, 1.0f) },

			{ Vector3f(0.0f, 0.0f, 0.0f), Vector4f(0.0f, 0.0f, 1.0f, 1.0f) },
			{ Vector3f(0.0f, 0.0f, 1.0f), Vector4f(0.0f, 0.0f, 1.0f, 1.0f) },
		};

		auto pVertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));
		VertexLayout layout = {
			{ VertexDataType::Float3, "a_position", },
			{ VertexDataType::Float4, "a_color", },
		};
		pVertexBuffer->setVertexLayout(layout);

		uint32_t indices[] = { 0, 1, 2, 3, 4, 5 };

		auto pIndexBuffer = IndexBuffer::create(indices, sizeof(indices) / sizeof(indices[0]));

		s_data.m_pVA = VertexArray::create(pVertexBuffer, pIndexBuffer);
		s_data.m_pShader->bind();
		s_data.m_pShader->setMat4("u_transform", Matrix4f(1.0f));
	}

	void Gizmos::shuntdown()
	{
		s_data.m_pShader.reset();
		s_data.m_pVA.reset();
	}

	void Gizmos::drawAxis(const Vector3f& position_)
	{
		auto transform = MyMath::translate(position_);
		RendererDebug::drawAxis_XYZ(transform);
	}
}