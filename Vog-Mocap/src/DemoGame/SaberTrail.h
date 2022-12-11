#pragma once
#include <Vog.h>

#include "ColorType.h"

#include <Vog/Graphics/Features/Trail.h>

namespace demo_game
{
	//#define ENABLE_SABER_DEBUG_LOG

	using namespace vog;
	class SaberTrail : public NativeScriptEntity
	{
	public:
		struct Data
		{
			Trail* pTrail = nullptr;
			uint32_t target = 0;
		};
		Data m_data;
		float m_timer = 0.0f;

	public:
		SaberTrail() = default;
		virtual ~SaberTrail()
		{
			m_data.pTrail = nullptr;
			m_data.target = 0;
		}

		virtual void onStart()
		{
			Data* pData = (Data*)getComponent<NativeScriptComponent>().pUeserData;
			m_data.pTrail = pData->pTrail;
			m_data.target = pData->target;

			VOG_ASSERT(m_data.target != 0, "");
		};

		virtual void onDestroy()
		{

		};

		virtual void onUpdate(float dt_)
		{
			if (m_timer > 2.0f)
			{
				//pTrail->addPoint(getComponent<TransformComponent>().translation);
				m_timer = 0.0f;
			}
			Entity targetEntity = { m_data.target, m_entity };
			auto& transform = targetEntity.getComponent<TransformComponent>();

			//m_data.pTrail->update_method0(dt_, transform.translation, MyMath::toQuaternion(transform.rotation) * Vector3f(1.0f, 0.0f, 0.0f));
			m_data.pTrail->update_method1(dt_, transform.translation, MyMath::toQuaternion(transform.rotation) * Vector3f(0.0f, 1.0f, 0.0f), MyMath::toQuaternion(transform.rotation));

			getComponent<MeshComponent>().indexCount = m_data.pTrail->getIndexCount();

			m_timer += dt_;
		};

		virtual void onTriggerEnter(uint32_t handle_) override
		{

		};

		virtual void onTriggerStay(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
			//VOG_CORE_LOG_INFO("Move Entity: onTriggerStay!");
		};

		virtual void onTriggerExit(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };

		};

	private:
	};
}