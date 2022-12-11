#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Graphics/RenderPass/RenderPass.h"
#include "Vog/Graphics/Renderer/Renderer.h"

// References:
// https://github.com/SimpleTalkCpp/workshop-2021-07-unity-shader/tree/main/Assets/Week002/Week002%20-%20Trail

namespace vog {

	struct TrailVertexLayout
	{
		Vector3f position;
		Vector2f uv;
	};

	struct TrailNode
	{
		Vector3f position0 = Vector3f(0.0f);
		Vector3f position1 = Vector3f(0.0f);

		float lifeTime = 0.0f;
		bool isAdded = false;
	};

	class VOG_API Trail : public NonCopyable
	{
	public:
		Trail() = default;
		~Trail();

		void init();
		void destroy();

		void update_method0(float dt_, const Vector3f& point_, const Vector3f& direction_, const Quaternion& orientation_);
		void update_method1(float dt_, const Vector3f& point_, const Vector3f& direction_, const Quaternion& orientation_);		// this is using reference method

		//void update(float dt_);




		void onImGuiRender();

		uint32_t getIndexCount();

	public:

		uint32_t verticesCount = 0;

		RefPtr<VertexBuffer> pVertexBuffer;
		RefPtr<IndexBuffer> pIndexBuffer;
		RefPtr<Shader> pShader;

		std::vector<TrailNode> nodes;

		Vector3f offset = Vector3f(0.0f);
		float width = 2.0f;
		float lifeTime = 1.0f;
		float tolerance = 0.1f;

	private:
		void _addPoint(const Vector3f& point_, const Vector3f& direction_);
		void _split(TrailNode& leftNode_, TrailNode& rightNode_, int depth_);

		void _addNode(TrailNode& newNode_, int depth_);

		void _setupMesh();

	private:
		static constexpr int s_max_node_count = 128;
		static constexpr int s_max_vertices_count = s_max_node_count * 2;
		static constexpr int s_max_indices_count = (s_max_node_count - 1) * 6;

		//ScopePtr<TrailVertexLayout[]> m_pVertices = nullptr;
		TrailVertexLayout* m_itVertices = nullptr;
		TrailVertexLayout* m_pVertices = nullptr;
	};

	inline uint32_t Trail::getIndexCount()
	{
		if (nodes.size() >= s_max_node_count)
			return (s_max_node_count - 1) * 6;
		else if (nodes.size() < 1)
			return 0;
		else
			return static_cast<uint32_t>((nodes.size() - 1) * 6);
	}

}