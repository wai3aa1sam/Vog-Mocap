#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Scene/Entity.h"

#include "Vog/Physics/Collision/CollisionCallbackBase.h"

namespace vog {

	// using this kind of inheritance may be hard to decoupled when the physics library is changed (could be used as data member to avoid the problem), 

	class VOG_API NativeScriptEntity : public CollisionCallbackBase
	{
		friend class Scene;
		friend struct NativeScriptSystem;
	public:
		NativeScriptEntity() = default;
		virtual ~NativeScriptEntity() = default;

		virtual void onAwake() {};

		virtual void onStart() {};
		virtual void onDestroy() {};

		virtual void onUpdate(float dt_) {};

		virtual void onTriggerEnter() {};
		//virtual void onTriggerStay() {};
		//virtual void onTriggerExit() {};


		template<typename T>
		T& getComponent()
		{
			return m_entity.getComponent<T>();
		}
	protected:
		Entity m_entity;
	};

}