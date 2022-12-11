#include "vogpch.h"
#include "ECS.h"

namespace vog::ecs
{
	static const uint32_t s_initial_component_type_count = 12;
	IDType TypeIDGenerator::m_count = 0;

#pragma region ECS_func
	ECS::ECS()
	{
		m_componentMangers.reserve(s_initial_component_type_count);
		registerComponent<HierarchyComponent>();

		getComponentManager<HierarchyComponent>().reserve(1000);		// TODO: remove, just a temporary solution
	}

	ECS::~ECS()
	{
		//VOG_CORE_LOG_TRACE("ECS dtor");
		shutdown();
	}

	ECS::ECS(const ECS& rhs_)
		:
		m_componentType2IndexMap(rhs_.m_componentType2IndexMap), m_entities(rhs_.m_entities), m_entity2IndexMap(rhs_.m_entity2IndexMap)
	{
		size_t cm_size = rhs_.m_componentMangers.size();
		m_componentMangers.reserve(cm_size);
		for (size_t i = 0; i < cm_size; i++)
		{
			m_componentMangers.emplace_back(rhs_.m_componentMangers[i]->clone());
		}
	}

	void ECS::operator=(const ECS& rhs_)
	{
		shutdown();

		m_componentType2IndexMap = rhs_.m_componentType2IndexMap;
		m_entities = rhs_.m_entities;
		m_entity2IndexMap = rhs_.m_entity2IndexMap;

		size_t cm_size = rhs_.m_componentMangers.size();
		m_componentMangers.reserve(cm_size);
		for (size_t i = 0; i < cm_size; i++)
		{
			m_componentMangers.emplace_back(rhs_.m_componentMangers[i]->clone());
		}
	}

	void ECS::shutdown()
	{
		VOG_CORE_ASSERT(m_componentMangers.size() == m_componentType2IndexMap.size(), "");

		for (auto& componentManger : m_componentMangers)
		{
			delete componentManger;
		}
		m_componentType2IndexMap.clear();
		m_entities.clear();
		m_entity2IndexMap.clear();
		m_componentMangers.clear();
	}

	EntityHandle ECS::createEntity()
	{
		VOG_CORE_ASSERT(m_entities.size() == m_entity2IndexMap.size(), "");

		static EntityHandle next = 1;
		m_entity2IndexMap[next] = m_entities.size();
		m_entities.push_back(next);
		return next++;
	}

	void ECS::destroyEntity(EntityHandle handle_)
	{
		VOG_CORE_ASSERT(m_entities.size() == m_entity2IndexMap.size(), "");
		VOG_CORE_ASSERT(m_entity2IndexMap.find(handle_) != m_entity2IndexMap.end(), "");

		auto it = m_entity2IndexMap.find(handle_);
		if (it == m_entity2IndexMap.end())
			return;

		_detach_and_remove_withChildren(handle_);

		const auto remove_index = it->second;
		const auto remove_entity = m_entities[remove_index];

		const auto lastIndex = m_entities.size() - 1;
		if (remove_index != lastIndex)
		{
			m_entities[remove_index] = m_entities[lastIndex];
			m_entity2IndexMap[m_entities[remove_index]] = remove_index;
		}

		m_entities.pop_back();
		m_entity2IndexMap.erase(remove_entity);
	}

	void ECS::destroyEntity_sorted(EntityHandle handle_)
	{
		VOG_CORE_ASSERT(m_entities.size() == m_entity2IndexMap.size(), "");
		VOG_CORE_ASSERT(m_entity2IndexMap.find(handle_) != m_entity2IndexMap.end(), "");

		auto it = m_entity2IndexMap.find(handle_);
		if (it == m_entity2IndexMap.end())
			return;

		_detach_and_remove_withChildren(handle_);

		const auto remove_index = it->second;
		const auto remove_entity = m_entities[remove_index];

		const auto size = m_entities.size();
		if (remove_index < size - 1)
		{
			for (size_t i = remove_index; i < size - 1; i++)
			{
				m_entities[i] = m_entities[i + 1];
				m_entity2IndexMap[m_entities[i]] = i;
			}
		}

		m_entities.pop_back();
		m_entity2IndexMap.erase(remove_entity);
	}

	void ECS::attach(EntityHandle entity_, EntityHandle parent_)
	{
		VOG_CORE_ASSERT(entity_ != parent_, "");
		VOG_CORE_ASSERT(m_entity2IndexMap.find(parent_) != m_entity2IndexMap.end() || parent_ == ecs::s_invalid_entity, "");
		VOG_CORE_ASSERT(m_entity2IndexMap.find(entity_) != m_entity2IndexMap.end(), "");

		auto& hierarchies = getComponentManager<HierarchyComponent>();

		VOG_CORE_ASSERT(!hierarchies.contain(entity_), "");

		auto& hierarchy = hierarchies.create(entity_);
		hierarchy.parentID = parent_;

		if (parent_ == s_invalid_entity)
			return;

		// should check nullptr
		auto& parent_hier = hierarchies.getComponent_unsafe(parent_);
		parent_hier.children.push_back(entity_);
	}

	void ECS::attach_withChildren(EntityHandle entity_, EntityHandle parent_)
	{
		VOG_CORE_ASSERT(entity_ != parent_, "");

		auto& hierarchies = getComponentManager<HierarchyComponent>();

		hierarchies.create(entity_).parentID = parent_;

		//for (size_t i = 0; i < hierarchies_.size(); i++)
		//{
		//	const HierarchyComponent& child = hierarchies_.m_components[i];

		//	if (child.parentID == entity_)
		//	{
		//		moveItem(hierarchies_.getCount() - 1, i);
		//		break;
		//	}
		//}

		const auto count = hierarchies.getCount();

		if (count < 2)
			return;

		for (size_t j = count - 1; j > 0; --j)
		{
			const auto parentEntity = hierarchies.getEntityHandle(j);

			for (size_t i = 0; i < count; ++i)
			{
				const HierarchyComponent& childHierarchy = hierarchies.m_components[i];

				if (childHierarchy.parentID == parentEntity)
				{
					hierarchies.moveItem(j, i);		// move j to i, i and its later entity will move to right
					++j;							// keep find child for the new parent entity ( find child of child (parent perspective) )
					break;
				}
			}
		}
	}

	void ECS::detach_withChildren(EntityHandle parent_)
	{
		auto& hierarchies = getComponentManager<HierarchyComponent>();

		_detach(hierarchies, parent_);

		auto& parent_hierarchy = *hierarchies.getpComponent(parent_);
		const auto parent_parentId = parent_hierarchy.parentID;

		if (parent_parentId == s_invalid_entity)
		{
			hierarchies.remove(parent_);
		}
		else			// if have parent
		{
			auto& parent_parentHier = *hierarchies.getpComponent(parent_parentId);
			parent_parentHier.remove_child_sorted(parent_);
			hierarchies.remove(parent_);
		}
	}

	void ECS::_detach(ComponentManager<HierarchyComponent>& hierarchies_, EntityHandle parent_)
	{
		auto& hierarchies = hierarchies_;

		auto& parent_hierarchy = *hierarchies.getpComponent(parent_);

		auto& children = parent_hierarchy.children;
		for (size_t i = 0; i < children.size(); i++)
		{
			const auto childEntity = children[i];
			_detach(hierarchies, childEntity);
			hierarchies.remove(childEntity);
		}
	}

	void ECS::_detach_and_remove_withChildren(EntityHandle parent_)
	{
		auto& hierarchies = getComponentManager<HierarchyComponent>();

		_detach_and_remove(hierarchies, parent_);

		auto& parent_hierarchy = *hierarchies.getpComponent(parent_);
		const auto parent_parentId = parent_hierarchy.parentID;

		if (parent_parentId != s_invalid_entity)
		{
			auto& parent_parentHier = *hierarchies.getpComponent(parent_parentId);
			parent_parentHier.remove_child_sorted(parent_);
		}

		for (auto& componentManger : m_componentMangers)
		{
			componentManger->remove(parent_);
		}
	}

	void ECS::_detach_and_remove(ComponentManager<HierarchyComponent>& hierarchies_, EntityHandle parent_)
	{
		auto& hierarchies = hierarchies_;

		auto& parent_hierarchy = *hierarchies.getpComponent(parent_);

		auto& children = parent_hierarchy.children;
		for (size_t i = 0; i < children.size(); i++)
		{
			const auto childEntity = children[i];
			_detach_and_remove(hierarchies, childEntity);

			for (auto& componentManger : m_componentMangers)
			{
				componentManger->remove(childEntity);
			}
		}
	}
#pragma endregion
}
