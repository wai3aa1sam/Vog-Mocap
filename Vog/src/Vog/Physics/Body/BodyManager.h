#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Collision/CollisionResult.h"

#include <vector>
#include <unordered_map>

/// <summary>
/// This class is incomplete!
/// </summary>

namespace vog {

	class CollisionManager;

	class VOG_API BodyManager : public NonCopyable
	{
	public:
		BodyManager();
		~BodyManager();

		void destroy();

		void reserve(size_t capacity_);

		Body* addBody();
		
		BodyHandle addBody_request();
		void removeBody_request(BodyHandle handle_);

		void updateProperties();
		void checkCollision(CollisionManager& collisionManager_);

		void updateHandles();

		size_t getSize() { return m_bodyPtrs.size(); }
		std::vector<Body*>& getBodies() { return m_bodyPtrs; };

	private:
		void removeBody(BodyHandle handle_);

	private:
		std::vector<BodyHandle> m_bodyHandles;
		std::unordered_map<BodyHandle, size_t> m_handle2IndexMap;
		std::vector<Body*> m_bodyPtrs;

		std::vector<BodyHandle> m_deadHandles;
		//std::vector<BodyHandle> m_newHandles;
	};
}