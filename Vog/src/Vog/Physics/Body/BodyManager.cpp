#include "vogpch.h"
#include "BodyManager.h"

#include "Vog/Physics/Collision/CollisionManager.h"

namespace vog {
	BodyManager::BodyManager()
	{
	}
	BodyManager::~BodyManager()
	{
		destroy();
	}

	void BodyManager::destroy()
	{
		for (auto* pBody : m_bodyPtrs)
		{
			delete pBody;
		}
		m_bodyPtrs.clear();
		m_handle2IndexMap.clear();
		m_bodyHandles.clear();
		m_deadHandles.clear();
	}

	void BodyManager::reserve(size_t capacity_)
	{
		m_handle2IndexMap.reserve(capacity_);
		m_bodyPtrs.reserve(capacity_);
		m_bodyHandles.reserve(capacity_);
	}

	Body* BodyManager::addBody()
	{
		static BodyHandle next = 1;

		Body* pNewBody = new Body;			// actually no need to use pointer and return the handle to user
		pNewBody->m_handle = next;
		m_handle2IndexMap[next] = m_bodyPtrs.size();
		m_bodyPtrs.emplace_back(pNewBody);
		m_bodyHandles.push_back(next);
		next++;
		return pNewBody;
	}

	void BodyManager::removeBody(BodyHandle handle_)
	{
		VOG_CORE_ASSERT(handle_ != s_invalid_body, "");

		//VOG_CORE_ASSERT(m_entities.size() > 0 && m_components.size() > 0, "");
		VOG_CORE_ASSERT(m_bodyHandles.size() == m_bodyPtrs.size(), "");
		VOG_CORE_ASSERT(m_handle2IndexMap.size() == m_bodyPtrs.size(), "");

		auto it = m_handle2IndexMap.find(handle_);
		if (it == m_handle2IndexMap.end())
		{
			return;
		}

		const auto remove_index = it->second;
		const auto remove_entity = m_bodyHandles[remove_index];

		Body* remove_pBody = m_bodyPtrs[remove_index];

		const auto lastIndex = m_bodyPtrs.size() - 1;
		if (remove_index < lastIndex)
		{
			m_bodyPtrs[remove_index] = std::move(m_bodyPtrs[lastIndex]);		// user should define proper move ctr/assign to prevent memory leak

			m_bodyHandles[remove_index] = m_bodyHandles[lastIndex];

			m_handle2IndexMap[m_bodyHandles[remove_index]] = remove_index;
		}

		delete remove_pBody;
		m_bodyPtrs.pop_back();
		m_bodyHandles.pop_back();
		m_handle2IndexMap.erase(remove_entity);
	}
	BodyHandle BodyManager::addBody_request()
	{
		assert(0);
		//static_assert(false);
		return BodyHandle();
	}
	void BodyManager::removeBody_request(BodyHandle handle_)
	{
		m_deadHandles.push_back(handle_);
	}
	void BodyManager::updateProperties()
	{
		for (size_t i = 0; i < m_bodyPtrs.size(); i++)
		{
			if (m_bodyPtrs[i]->m_pShape)
				m_bodyPtrs[i]->m_pShape->updateProperties(m_bodyPtrs[i]->m_transform);
			else
				VOG_CORE_LOG_WARN("Rigidbody has no shape!");
		}
	}
	void BodyManager::checkCollision(CollisionManager& collisionManager_)
	{
		for (size_t i = 0; i < m_bodyPtrs.size(); i++)
		{
			for (size_t j = i + 1; j < m_bodyPtrs.size(); j++)
			{
				if (!m_bodyPtrs[i]->m_pShape || !m_bodyPtrs[j]->m_pShape)
					continue;

				if (!m_bodyPtrs[i]->m_isEnable || !m_bodyPtrs[j]->m_isEnable)
					continue;

				auto* pShape_A = m_bodyPtrs[i]->m_pShape;
				auto* pShape_B = m_bodyPtrs[j]->m_pShape;

				auto& transform_A = m_bodyPtrs[i]->m_transform;
				auto& transform_B = m_bodyPtrs[j]->m_transform;

				CollisionResult ret = pShape_A->collide(&transform_A, pShape_B, &transform_B);

				if (ret.isCollided)
				{
					ret.pBody_A = m_bodyPtrs[i];
					ret.pBody_B = m_bodyPtrs[j];

					collisionManager_.m_collisionResults.emplace_back(ret);
				}
				else if (collisionManager_.isCollisionExist(m_bodyPtrs[i]->m_handle, m_bodyPtrs[j]->m_handle))
				{
					collisionManager_.invokeCollisionExitCallback(*m_bodyPtrs[i], *m_bodyPtrs[j]);
				}
			}
		}
	}

	void BodyManager::updateHandles()
	{
		/*if (m_deadHandles.size() > 0)
		{
			VOG_CORE_LOG_INFO("dead size: {0}", m_deadHandles.size());
		}*/
		for (size_t i = 0; i < m_deadHandles.size(); i++)
		{
			removeBody(m_deadHandles[i]);
		}
		m_deadHandles.clear();
	}
}