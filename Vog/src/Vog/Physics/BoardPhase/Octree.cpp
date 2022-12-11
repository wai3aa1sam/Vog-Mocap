#include "vogpch.h"
#include "Octree.h"

#include "Vog/Physics/Collision/Shapes/AABBShape.h"

#include "Vog/Physics/Collision/CollisionResult.h"

//#define VOG_OCTREE_DEBUG

namespace vog {

	OctreeNode::OctreeNode()
		:
		m_aabbBound({ -200.0f,-200.0f, -200.0f }, { 200.0f, 200.0f, 200.0f }), m_pChildren(nullptr)
	{
	}

	OctreeNode::~OctreeNode()
	{
		free();
	}

	void OctreeNode::free()
	{
		if (!isLeafNode())
		{
			for (int i = 0; i < s_objects_per_node; i++)
			{
				m_pChildren[i].free();
			}
			delete[] m_pChildren;
			m_pChildren = nullptr;
		}
	}

	void OctreeNode::splitTree(const std::vector<Body*>& bodyPtrs_, int depth_)
	{
		if (depth_-- <= 0)
			return;

		if (isLeafNode() && isNeedToSplit())
		{
			m_pChildren = new OctreeNode[s_children_per_node];

			const auto center = m_aabbBound.getCenter();
			const auto size = m_aabbBound.getSize() * 0.5f;

			std::array<Vector3f, s_children_per_node> newCenters = {
				center + Vector3f{ -size.x,  size.y,  size.z },		//
				center + Vector3f{  size.x,  size.y,  size.z },		//
				center + Vector3f{ -size.x, -size.y,  size.z },		//
				center + Vector3f{  size.x, -size.y,  size.z },		//
				center + Vector3f{ -size.x,  size.y, -size.z },		//
				center + Vector3f{  size.x,  size.y, -size.z },		//
				center + Vector3f{ -size.x, -size.y, -size.z },		//
				center + Vector3f{  size.x, -size.y, -size.z },		//
			};
			for (int i = 0; i < s_children_per_node; i++)
				m_pChildren[i].m_aabbBound.set(newCenters[i] - size, newCenters[i] + size);
		}

		if (!isLeafNode() && isNeedToSplit())
		{
			for (int bodyIndex = 0; bodyIndex < m_objectIDs.size(); bodyIndex++)
			{
				for (int childrenIndex = 0; childrenIndex < s_children_per_node; childrenIndex++)
				{
					if (m_pChildren[childrenIndex].isInBoundary(m_objectIDs[bodyIndex], bodyPtrs_))	// if object is in the new child aabb, add to this child, and continue next object
					{
						m_pChildren[childrenIndex].m_objectIDs.push_back(m_objectIDs[bodyIndex]);

#ifdef VOG_OCTREE_DEBUG
						VOG_CORE_LOG_INFO("spilt: intersected with oct node, object id: {0}", m_objectIDs[bodyIndex]);
						VOG_CORE_LOG_INFO("current object size: {0}", m_pChildren[childrenIndex].m_objectIDs.size());
#endif // VOG_OCTREE_DEBUG
						//break;
					}

					//VOG_CORE_ASSERT(childrenIndex != s_children_per_node - 1, "");		// at least one of its child should collide with that object
				}
			}

			m_objectIDs.clear();
			m_objectIDs.shrink_to_fit();

			for (int i = 0; i < s_children_per_node; i++)
			{
				m_pChildren[i].splitTree(bodyPtrs_, depth_);
			}
		}

	}

	void OctreeNode::insert(OctreeNode* pNode_, int id_, const std::vector<Body*>& bodyPtrs_)
	{
		_insert(pNode_, id_, bodyPtrs_);
	}
	void OctreeNode::remove(OctreeNode* pNode_, int id_)
	{
		_remove(pNode_, id_);
	}
	void OctreeNode::update(std::vector<Body*> bodyPtrs_)
	{
#ifdef VOG_OCTREE_DEBUG
		VOG_CORE_LOG_INFO("=======Start=========");
		print();
#endif // VOG_OCTREE_DEBUG

		for (size_t i = 0; i < bodyPtrs_.size(); i++)
		{
			remove(this, bodyPtrs_[i]->m_handle);
			insert(this, bodyPtrs_[i]->m_handle, bodyPtrs_);
		}
		splitTree(bodyPtrs_, s_max_depth_per_spilt);

		size_t totalObjectSize = getTotalObjectCount();

		//VOG_CORE_ASSERT(totalObjectSize == bodyPtrs_.size(), "");
#ifdef VOG_OCTREE_DEBUG
		VOG_CORE_LOG_INFO("=======End, Total Obejct size: {0}", getTotalObjectCount());
#endif // VOG_OCTREE_DEBUG

	}

	std::vector<CollisionResult> OctreeNode::checkCollision(const std::vector<Body*>& bodyPtrs_)
	{
		std::vector<CollisionResult> results;

		_checkCollision(results, bodyPtrs_);

		return results;
	}


	bool OctreeNode::isInBoundary(int index_, std::vector<Body*> bodyPtrs_)
	{
		BodyTransform boundingTransform;
		boundingTransform.position = m_aabbBound.getCenter();
		AABBShape boundingShape({0.0f, 0.0f, 0.0f}, m_aabbBound.getSize());

		auto* pBodyShape = bodyPtrs_[index_]->m_pShape;
		auto& bodyTransform = bodyPtrs_[index_]->m_transform;

#ifdef VOG_OCTREE_DEBUG
		/*VOG_CORE_LOG_INFO("Trying to check intersection, {0} with {1}", pBodyShape->getName(), boundingShape.getName());
		VOG_CORE_LOG_INFO("Bounding center:");
		VOG_CORE_LOG_GLM(boundingTransform.position);
		VOG_CORE_LOG_INFO("Bounding size:");
		VOG_CORE_LOG_GLM(m_aabbBound.getSize());*/
#endif // VOG_OCTREE_DEBUG

		return pBodyShape->collide(&bodyTransform, &boundingShape, &boundingTransform);
	}

	void OctreeNode::print(int depth_)
	{
		if (isLeafNode())
		{
			VOG_CORE_LOG_INFO("{0} level: {1}, object size: {2}", getIndentationLevel(depth_), depth_, m_objectIDs.size());
			std::string ids;
			for (size_t i = 0; i < m_objectIDs.size(); i++)
			{
				ids += std::to_string(m_objectIDs[i]);
				ids += ", ";
			}
			VOG_CORE_LOG_INFO("ids: {0}", ids);
		}
		else
		{
			VOG_CORE_ASSERT(m_objectIDs.empty(), "");
			VOG_CORE_LOG_INFO("Parent {0} level: {1}", getIndentationLevel(depth_), depth_);

			int next_depth = depth_ + 1;
			for (int i = 0; i < s_children_per_node; i++)
			{
				m_pChildren[i].print(next_depth);
			}
		}
	}

	size_t OctreeNode::getTotalObjectCount()
	{
		assert(0);
		return 0;
		if (!isLeafNode())
		{
			size_t totalSize = 0;
			for (int i = 0; i < s_children_per_node; i++)
			{
				totalSize += m_pChildren[i].getTotalObjectCount();
			}
			return totalSize;
		}

		if (isLeafNode())
		{
			return m_objectIDs.size();
		}
	}

	void OctreeNode::_insert(OctreeNode* pNode_, int id_, std::vector<Body*> bodyPtrs_)
	{
		if (pNode_->isLeafNode())
		{
			if (pNode_->isInBoundary(id_, bodyPtrs_))
			{
				pNode_->m_objectIDs.push_back(id_);
#ifdef VOG_OCTREE_DEBUG
				VOG_CORE_LOG_INFO("insert: intersected with oct node, object id: {0}", id_);
				VOG_CORE_LOG_INFO("current object size: {0}", pNode_->m_objectIDs.size());
#endif // VOG_OCTREE_DEBUG

				//bool isIntersected = pNode_->isInBoundary(id_, bodyPtrs_);
				//if (isIntersected)	// if intersect
				//{
				//	pNode_->m_objectIDs.push_back(id_);


				//}
				//return isIntersected;
			}
		}
		else
		{
			for (int i = 0; i < s_children_per_node; i++)
			{
				_insert(&pNode_->m_pChildren[i], id_, bodyPtrs_);
			}
		}
	}

	void OctreeNode::_remove(OctreeNode* pNode_, int id_)
	{
		if (pNode_->isLeafNode())
		{
			auto it = std::find(pNode_->m_objectIDs.begin(), pNode_->m_objectIDs.end(), id_);
			if (it != pNode_->m_objectIDs.end())	// if intersect
			{
#ifdef VOG_OCTREE_DEBUG
				VOG_CORE_LOG_INFO("remove: found in oct node, object id: {0}", id_);
#endif // VOG_OCTREE_DEBUG
				pNode_->m_objectIDs.erase(it);
				//return true;
			}
			//return false;
			//return it != m_objectIDs.end();		// return ture if found
		}
		else
		{
			for (int i = 0; i < s_children_per_node; i++)
			{
				_remove(&pNode_->m_pChildren[i], id_);
			}
		}
	}

	void OctreeNode::_checkCollision(std::vector<CollisionResult>& results, const std::vector<Body*>& bodyPtrs_)
	{
		if (isLeafNode())
		{
			// check
			for (size_t i = 0; i < m_objectIDs.size(); i++)
			{
				for (size_t j = i + 1; j < m_objectIDs.size(); j++)
				{
					auto id_A = m_objectIDs[i];
					auto id_B = m_objectIDs[j];

					auto* pBody_A = bodyPtrs_[id_A];
					auto* pBody_B = bodyPtrs_[id_B];


					if (!pBody_A->m_pShape || !pBody_B->m_pShape)
						continue;

					auto* pShape_A = pBody_A->m_pShape;
					auto* pShape_B = pBody_B->m_pShape;

					auto& transform_A = pBody_A->m_transform;
					auto& transform_B = pBody_B->m_transform;

					CollisionResult ret = pShape_A->collide(&transform_A, pShape_B, &transform_B);

					if (ret.isCollided)
					{
						ret.pBody_A = pBody_A;
						ret.pBody_B = pBody_B;

						results.emplace_back(ret);
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < s_children_per_node; i++)
			{
				m_pChildren[i]._checkCollision(results, bodyPtrs_);
			}
		}
	}

#if 0
	void OctreeNode::splitTree(OctreeNode* pNode_, int depth_)
	{
		if (depth_-- <= 0)
			return;

		if (isLeafNode())
		{
			m_pChildren = new OctreeNode[s_children_per_node];

			const auto center = m_aabbBound.getCenter();
			const auto size = m_aabbBound.getSize() / 2.0f;

			std::array<Vector3f, s_children_per_node> newCenters = {
				center + Vector3f{ -size.x,  size.y,  size.z },
				center + Vector3f{ +size.x,  size.y,  size.z },
				center + Vector3f{ -size.x, -size.y,  size.z },
				center + Vector3f{ +size.x, -size.y,  size.z },
				center + Vector3f{ -size.x,  size.y, -size.z },
				center + Vector3f{ +size.x,  size.y, -size.z },
				center + Vector3f{ -size.x, -size.y,  size.z },
				center + Vector3f{ +size.x, -size.y,  size.z },
			};
			for (int i = 0; i < s_children_per_node; i++)
				m_pChildren[i].m_aabbBound.set(newCenters[i] - size, newCenters[i] + size);

			for (int i = 0; i < m_objectIDs.size(); i++)
			{
				for (int j = 0; j < s_children_per_node; j++)
				{
					if (true)	// if object is in the new child aabb, add to this child, and continue next object
					{
						m_pChildren[i].m_objectIDs.push_back(m_objectIDs[i]);
						break;
					}
				}
			}

			m_objectIDs.shrink_to_fit();

			for (int i = 0; i < s_children_per_node; i++)
			{
				if (m_pChildren[i].isNeedToSplit())	// if object is in the new child aabb, add to this child, and continue next object
				{
					splitTree(&m_pChildren[i], depth_);
				}
			}
		}
	}

	void OctreeNode::insert(OctreeNode* pNode_, int id_)
	{
		_insert(pNode_, id_);
	}
	bool OctreeNode::_insert(OctreeNode* pNode_, int id_)
	{
		if (isLeafNode())
		{
			if (!isNeedToSplit())
			{
				bool isIntersected = true;

				if (isIntersected)	// if intersect
				{
					m_objectIDs.push_back(id_);
				}
				return isIntersected;
			}
			else
			{
				splitTree(this, s_max_depth_per_spilt);

				for (int i = 0; i < s_children_per_node; i++)
				{
					if (_insert(&m_pChildren[i], id_))
					{
						return true;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < s_children_per_node; i++)
			{
				if (_insert(&m_pChildren[i], id_))
				{
					return true;
				}
			}
		}
	}
#endif // 0
}
