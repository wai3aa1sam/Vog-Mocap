#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Debug/BenchmarkTimer.h"

#include <vector>
#include <unordered_map>
#include <new>

#include <chrono>
#include <iostream>

// references:
// https://wickedengine.net/2019/09/29/entity-component-system/
// https://indiegamedev.net/2020/05/19/an-entity-component-system-with-data-locality-in-cpp/
// https://austinmorlan.com/posts/entity_component_system/#the-component
// https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html

/*
*	Important: if appending ecs stuff, espcailly that Component has vector(dynamic array) (), (eg. referencing the HierarchyComponent in duplicate or create Entity in scene)
*	the vector in that ComponentManager maybe resized, getting the pointer of that maybe invaild after operations (especially recursion)
*/

namespace vog::ecs {

	using IDType = uint32_t;
	using EntityHandle = uint32_t;
	using ComponentIDType = uint32_t;

	static const EntityHandle s_invalid_entity = 0;

	class TypeIDGenerator
	{
	public:
		template<class U>
		static const IDType getNewID()
		{
			static const IDType idCounter = m_count++;
			return idCounter;
		}
	private:
		static IDType m_count;
	};

	//template<class T> IDType TypeIDGenerator<T>::m_count = 0;

	struct HierarchyComponent
	{
		EntityHandle parentID = s_invalid_entity;
		std::vector<EntityHandle> children;

		HierarchyComponent() = default;
		HierarchyComponent(HierarchyComponent& rhs_) = default;
		void operator=(const HierarchyComponent& rhs_)
		{
			if (this == &rhs_)
				return;
			if (!children.empty())
			{
				children.clear();
			}
			children = std::move(rhs_.children);
			parentID = rhs_.parentID;
		}

		HierarchyComponent(HierarchyComponent&& rhs_)
		{
			children = std::move(rhs_.children);
			parentID = rhs_.parentID;

			rhs_.parentID = s_invalid_entity;
		}
		void operator=(HierarchyComponent&& rhs_)
		{
			if (this == &rhs_)
				return;
			if (!children.empty())
			{
				children.clear();
			}
			children = std::move(rhs_.children);
			parentID = rhs_.parentID;

			rhs_.parentID = s_invalid_entity;
		}

		void attach_child(EntityHandle handle_)
		{
			children.push_back(handle_);
		}

		void remove_child_sorted(EntityHandle handle_)
		{
			for (size_t i = 0; i < children.size(); i++)
			{
				if (handle_ == children[i])
					children.erase(children.begin() + i);
			}
		}
	};

	class VOG_API IComponent
	{
	public:
		virtual ~IComponent() = default;
		virtual void remove(EntityHandle handle_) = 0;
		virtual void remove_sorted(EntityHandle handle_) = 0;

		virtual IComponent* clone() = 0;
	};

	template<typename ComponentType>
	class ComponentManager : public IComponent
	{
		friend class ECS;
	public:
		ComponentManager() = default;
		virtual ~ComponentManager();

		virtual IComponent* clone() override;

		//ComponentManager(const ComponentManager& rhs_);
		//void operator=(const ComponentManager& rhs_);

		void reserve(size_t capacity_);

		template<typename... Args>
		ComponentType& create(EntityHandle handle_, Args&&... args_);

		virtual void remove(EntityHandle handle_) override;				// breaks orignal entity order
		virtual void remove_sorted(EntityHandle handle_) override;		// keeps orignal entity order

		void moveItem(size_t IndexFrom_, size_t IndexTo_);

		ComponentType* getData();
		ComponentType* getData() const;

		bool contain(EntityHandle handle_) const { return m_entity2IndexMap.find(handle_) != m_entity2IndexMap.end(); }

		std::vector<ComponentType>& getComponents() { return m_components; }
		const std::vector<ComponentType>& getComponents() const { return m_components; }

		ComponentType& getComponent_unsafe(EntityHandle handle_)
		{
			auto it = m_entity2IndexMap.find(handle_);
			VOG_CORE_ASSERT(it != m_entity2IndexMap.end(), "");
			return m_components[it->second];
		}

		ComponentType* getpComponent(EntityHandle handle_)
		{ 
			auto it = m_entity2IndexMap.find(handle_);
			if (it == m_entity2IndexMap.end())
				return nullptr;
			return &m_components[it->second];
		}
		size_t getCount() { return m_components.size(); }
		EntityHandle getEntityHandle(size_t index_) const { return m_entities[index_]; }

		static ComponentIDType getComponentTypeID() { return TypeIDGenerator::getNewID<ComponentType>(); }

	private:
		std::vector<ComponentType> m_components;
		std::vector<EntityHandle> m_entities;
		std::unordered_map<EntityHandle, size_t> m_entity2IndexMap;
	};

	class VOG_API ECS //: public NonCopyable
	{
	public:
		ECS();
		~ECS();

		ECS(const ECS& rhs_);
		void operator=(const ECS& rhs_);

		void shutdown();

		EntityHandle createEntity();
		void destroyEntity(EntityHandle handle_);
		void destroyEntity_sorted(EntityHandle handle_);

		void attach(EntityHandle entity_, EntityHandle parent_);
		[[deprecated]]void attach_withChildren(EntityHandle entity_, EntityHandle parent_);

		//void detach(EntityHandle entity_);
		void detach_withChildren(EntityHandle parent_);

		template<typename T>
		ComponentManager<T>* registerComponent();

		template<typename T>
		void reserve(size_t capacity_);

		template<typename T, typename... Args>
		T& addEntityComponent(EntityHandle handle_, Args&&...args_);

		template<typename T>
		void removeEntityComponent(EntityHandle handle_);
		
		template<typename T>
		ComponentManager<T>& getComponentManager();

		template<typename T> const ComponentManager<T>& getComponentManager() const;

		std::vector<EntityHandle>& getEntities() { return m_entities; }
		const std::vector<EntityHandle>& getEntities() const { return m_entities; }

		template<typename Func>
		void for_eachEntity(Func func_) const;
		
		template<typename Func>
		void for_each_rootEntity(Func func_);

		template<typename T>
		T* getpComponent(EntityHandle handle_);

		template<typename T>
		T& getComponent_unsafe(EntityHandle handle_);

		template<typename T>
		inline bool isEntityHasComponent(EntityHandle handle_) const;

		template<typename T> inline bool hasComponentType() { return m_componentType2IndexMap.find(ComponentManager<T>::getComponentTypeID()) != m_componentType2IndexMap.end(); }
		template<typename T> inline bool hasComponentType() const { return m_componentType2IndexMap.find(ComponentManager<T>::getComponentTypeID()) != m_componentType2IndexMap.end(); }

		template<typename T> static ComponentIDType getComponentTypeID() { return ComponentManager<T>::getComponentTypeID(); }

	private:
		void _detach(ComponentManager<HierarchyComponent>& hierarchies_, EntityHandle parent_);

		void _detach_and_remove_withChildren(EntityHandle parent_);
		void _detach_and_remove(ComponentManager<HierarchyComponent>& hierarchies_, EntityHandle parent_);


	private:
		std::vector<IComponent*> m_componentMangers;
		std::unordered_map<ComponentIDType, size_t> m_componentType2IndexMap;

		std::vector<EntityHandle> m_entities;
		std::unordered_map<EntityHandle, size_t> m_entity2IndexMap;
		//std::vector<EntityHandle> m_deadEntities;
	};

#pragma region ECS_template_func
	template<typename T>
	inline ComponentManager<T>* ECS::registerComponent()
	{
		VOG_CORE_ASSERT(m_componentMangers.size() == m_componentType2IndexMap.size(), "");

		//BenchmarkTimer t;

		if (hasComponentType<T>())
		{
			VOG_CORE_LOG_ERROR("Component Type already exist!");
			return nullptr; // can't re-register a type
		}

		ComponentIDType componentTypeId = ComponentManager<T>::getComponentTypeID();

		m_componentType2IndexMap.emplace(componentTypeId, m_componentMangers.size());

		ComponentManager<T>* cm = new ComponentManager<T>;
		m_componentMangers.push_back(cm);
		componentTypeId++;
		return cm;
	}

	template<typename T>
	inline void ECS::reserve(size_t capacity_)
	{
		getComponentManager<T>.reserve(capacity_);
	}

	template<typename T, typename... Args>
	T& ECS::addEntityComponent(EntityHandle handle_, Args&&... args_)
	{
		auto& cm = getComponentManager<T>();
		return cm.create(handle_, std::forward<Args>(args_)...);
	}

	template<typename T>
	void ECS::removeEntityComponent(EntityHandle handle_)
	{
		ComponentManager<T>& cm = getComponentManager<T>();

		cm.remove(handle_);
	}

	template<typename T>
	inline ComponentManager<T>& ECS::getComponentManager()
	{
		if (!hasComponentType<T>())
			registerComponent<T>();

		VOG_CORE_ASSERT(m_componentMangers.size() > 0, "");
		VOG_CORE_ASSERT(hasComponentType<T>(), "");

		ComponentIDType componentTypeId = ComponentManager<T>::getComponentTypeID();
		size_t index = m_componentType2IndexMap.at(componentTypeId);
		auto* pIComponent = m_componentMangers[index];

		return *static_cast<ComponentManager<T>*>(pIComponent);
	}

	template<typename T>
	inline const ComponentManager<T>& ECS::getComponentManager() const
	{
		if (!hasComponentType<T>())
			const_cast<ECS*>(this)->registerComponent<T>();

		VOG_CORE_ASSERT(m_componentMangers.size() > 0, "");
		VOG_CORE_ASSERT(hasComponentType<T>(), "");

		ComponentIDType componentTypeId = ComponentManager<T>::getComponentTypeID();
		size_t index = m_componentType2IndexMap.at(componentTypeId);
		auto* pIComponent = m_componentMangers[index];

		return *static_cast<ComponentManager<T>*>(pIComponent);
	}

	template<typename Func>
	inline void ECS::for_eachEntity(Func func_) const
	{
		for (auto i = 0; i < m_entities.size(); ++i)
		{
			func_(m_entities[i]);
		}
	}

	template<typename Func>
	inline void ECS::for_each_rootEntity(Func func_)
	{
		auto& hierarchy_cm = getComponentManager<HierarchyComponent>();
		auto& hierarchies = hierarchy_cm.getComponents();
		for (size_t i = 0; i < hierarchies.size(); i++)
		{
			if (hierarchies[i].parentID == ecs::s_invalid_entity)		// if it is root
			{
				func_(hierarchy_cm.getEntityHandle(i));
			}
		}
	}
	
	template<typename T>
	inline T* ECS::getpComponent(EntityHandle handle_)
	{
		ComponentManager<T>& cm = getComponentManager<T>();
		VOG_CORE_ASSERT(cm.m_components.size() > 0, "");
		VOG_CORE_ASSERT(cm.m_components.size() == cm.m_entities.size(), "");
		VOG_CORE_ASSERT(cm.m_entities.size() == cm.m_entity2IndexMap.size(), "");

		return cm.getpComponent(handle_);
	}

	template<typename T>
	inline T& ECS::getComponent_unsafe(EntityHandle handle_)
	{
		ComponentManager<T>& cm = getComponentManager<T>();
		VOG_CORE_ASSERT(cm.m_components.size() > 0, "");
		VOG_CORE_ASSERT(cm.m_components.size() == cm.m_entities.size(), "");
		VOG_CORE_ASSERT(cm.m_entities.size() == cm.m_entity2IndexMap.size(), "");

		return cm.getComponent_unsafe(handle_);
	}

	template<typename T>
	inline bool ECS::isEntityHasComponent(EntityHandle handle_) const
	{
		const ComponentManager<T>& cm = getComponentManager<T>();

		return cm.contain(handle_);
	}
#pragma endregion

#pragma region ComponentManager_template_func
	template<typename T>
	inline ComponentManager<T>::~ComponentManager()
	{
		//VOG_CORE_LOG_TRACE("ComponentManager dtor");
	}

	template<typename ComponentType>
	inline IComponent* ComponentManager<ComponentType>::clone()
	{
		auto* newComponentManager = new ComponentManager<ComponentType>();
		newComponentManager->m_components = m_components;
		newComponentManager->m_entities = m_entities;
		newComponentManager->m_entity2IndexMap = m_entity2IndexMap;
		return newComponentManager;
	}

	template<typename ComponentType>
	inline void ComponentManager<ComponentType>::reserve(size_t capacity_)
	{
		this->m_components.reserve(capacity_);
		this->m_entities.reserve(capacity_);
		this->m_entity2IndexMap.reserve(capacity_);
	}

	template<typename ComponentType>
	template<typename... Args>
	inline ComponentType& ComponentManager<ComponentType>::create(EntityHandle handle_, Args&&...args_)
	{
		VOG_CORE_ASSERT(!contain(handle_), "");
		VOG_CORE_ASSERT(handle_ != s_invalid_entity, "");

		VOG_CORE_ASSERT(m_entities.size() == m_components.size(), "");
		VOG_CORE_ASSERT(m_entity2IndexMap.size() == m_components.size(), "");

		m_entity2IndexMap[handle_] = m_components.size();

		m_entities.push_back(handle_);

		//m_components.push_back(std::move(ComponentType(std::forward<Args>(args_)...)));
		m_components.emplace_back(std::forward<Args>(args_)...);

		return m_components.back();
	}

	template<typename T>
	inline void ComponentManager<T>::remove(EntityHandle handle_)
	{
		VOG_CORE_ASSERT(handle_ != s_invalid_entity, "");

		//VOG_CORE_ASSERT(m_entities.size() > 0 && m_components.size() > 0, "");
		VOG_CORE_ASSERT(m_entities.size() == m_components.size(), "");
		VOG_CORE_ASSERT(m_entity2IndexMap.size() == m_components.size(), "");

		auto it = m_entity2IndexMap.find(handle_);
		if (it == m_entity2IndexMap.end())
		{
			return;
		}

		const auto remove_index = it->second;
		const auto remove_entity = m_entities[remove_index];

		const auto lastIndex = m_components.size() - 1;
		if (remove_index < lastIndex)
		{
			//m_components[remove_index] = std::move(m_components[lastIndex]);

			// move the data
			//auto* removeAddress = m_components.data() + remove_index;
			//removeAddress->~T();
			//new (removeAddress) T(std::move(m_components[lastIndex]));
			m_components[remove_index] = std::move(m_components[lastIndex]);		// user should define proper move ctr/assign to prevent memory leak

			m_entities[remove_index] = m_entities[lastIndex];

			m_entity2IndexMap[m_entities[remove_index]] = remove_index;
		}

		m_components.pop_back();
		m_entities.pop_back();
		m_entity2IndexMap.erase(remove_entity);
	}

	template<typename ComponentType>
	inline void ComponentManager<ComponentType>::remove_sorted(EntityHandle handle_)
	{
		VOG_CORE_ASSERT(handle_ != s_invalid_entity, "");

		//VOG_CORE_ASSERT(m_entities.size() > 0 && m_components.size() > 0, "");
		VOG_CORE_ASSERT(m_entities.size() == m_components.size(), "");
		VOG_CORE_ASSERT(m_entity2IndexMap.size() == m_components.size(), "");

		auto it = m_entity2IndexMap.find(handle_);
		if (it == m_entity2IndexMap.end())
		{
			return;
		}

		const auto remove_index = it->second;
		const auto remove_entity = m_entities[remove_index];

		const auto size = m_components.size();
		if (remove_index < size - 1)
		{
			// two for loops for better locality
			// move all items (after remove_index) from right to left by 1
			for (size_t i = remove_index; i < size - 1; i++)
			{
				m_components[i] = std::move(m_components[i + 1]);		// user should define proper move ctr/assign to prevent memory leak
			}

			for (size_t i = remove_index; i < size; i++)
			{
				m_entities[i] = m_entities[i + 1];
				m_entity2IndexMap[m_entities[i]] = i;
			}
		}

		m_components.pop_back();
		m_entities.pop_back();
		m_entity2IndexMap.erase(remove_entity);
	}

	template<typename ComponentType>
	inline void ComponentManager<ComponentType>::moveItem(size_t IndexFrom_, size_t IndexTo_)
	{
		VOG_CORE_ASSERT(IndexFrom_ >= 0  && IndexFrom_ < m_components.size(), "");
		VOG_CORE_ASSERT(IndexTo_ >= 0 && IndexTo_ < m_components.size(), "");

		if (IndexFrom_ == IndexTo_)
			return;

		auto& components = m_components;
		auto& entities = m_entities;
		auto& lookupMap = m_entity2IndexMap;

		// copy / move to a temp 
		auto entityFrom = entities[IndexFrom_];
		auto componentFrom = std::move(components[IndexFrom_]);

		const size_t dir = IndexFrom_ < IndexTo_ ? 1 : -1;

		for (size_t i = IndexFrom_; i < IndexTo_; i += dir)
		{
			const size_t nextIndex = IndexFrom_ + dir;

			//components[i].~ComponentType();
			components[i] = std::move(components[nextIndex]);
			entities[i] = entities[nextIndex];
			lookupMap[entities[i]] = i;
		}

		components[IndexTo_] = std::move(componentFrom);
		entities[IndexTo_] = entityFrom;
		lookupMap[entityFrom] = IndexTo_;
	}

	template<typename ComponentType> inline ComponentType* ComponentManager<ComponentType>::getData()		{ return m_components.data(); }
	template<typename ComponentType> inline ComponentType* ComponentManager<ComponentType>::getData() const { return m_components.data(); }
#pragma endregion

}