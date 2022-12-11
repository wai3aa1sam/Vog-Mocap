#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

#include "Vog/Physics/Collision/Bounding/AABBBound.h"

#include "Vog/Physics/Body/Body.h"

#include <vector>

namespace vog {

	class VOG_API OctreeNode : public NonCopyable
	{
	public:
		static constexpr int s_objects_per_node = 2;
		static constexpr int s_children_per_node = 8;
		static constexpr int s_max_depth_per_spilt = 5;

		OctreeNode();
		~OctreeNode();

		void free();

		//void splitTree(OctreeNode* pNode_, int depth_);

		//void insert(OctreeNode* pNode_, int id_);

		void splitTree(const std::vector<Body*>& bodyPtrs_, int depth_ = s_max_depth_per_spilt);
		void insert(OctreeNode* pNode_, int id_, const std::vector<Body*>& bodyPtrs_);
		std::vector<CollisionResult> checkCollision(const std::vector<Body*>& bodyPtrs_);
		void update(std::vector<Body*> bodyPtrs_);

		void remove(OctreeNode* pNode_, int id_);

		bool isLeafNode() { return m_pChildren == nullptr; }
		bool isNeedToSplit() { return m_objectIDs.size() > s_objects_per_node; }

		bool isInBoundary(int index_, std::vector<Body*> bodyPtrs_);

		void print(int depth_ = 0);

		size_t getTotalObjectCount();

	private:
		//bool _insert(OctreeNode* pNode_, int id_);
		void _insert(OctreeNode* pNode_, int id_, std::vector<Body*> bodyPtrs_);

		void _remove(OctreeNode* pNode_, int id_);

		void _checkCollision(std::vector<CollisionResult>& results, const std::vector<Body*>& bodyPtrs_);


	private:

		AABBBound m_aabbBound;

		std::vector<int> m_objectIDs;

		OctreeNode* m_pChildren = nullptr;
	};
}