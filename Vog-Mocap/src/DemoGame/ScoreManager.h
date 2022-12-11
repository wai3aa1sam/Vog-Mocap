#pragma once
#include <Vog.h>
#include "RhythmCube.h"

namespace demo_game
{
	using namespace vog;
	class ScoreManager : public NativeScriptEntity
	{
	public:

	public:
		ScoreManager()
		{
			VOG_ASSERT(!s_pInstance, "")
		}
		virtual ~ScoreManager()
		{
			s_pInstance = nullptr;
		}

		ScoreManager& get() { VOG_ASSERT(s_pInstance, ""); return *s_pInstance; }

		virtual void onAwake() override;

		virtual void onStart() override
		{
			//VOG_CORE_LOG_INFO("Move Entity: onStart!");

		};

		virtual void onDestroy() override
		{

		};

		virtual void onUpdate(float dt_) override
		{
		};

		virtual void onTriggerEnter(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
		};

		virtual void onTriggerStay(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
		};

		virtual void onTriggerExit(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
		};

	private:
		static ScoreManager* s_pInstance;
	};
}