#pragma once
#include <Vog.h>
#include "RhythmCube.h"

#include "RhythmCubeManager.h"
#include "LightManager.h"
#include "ScoreManager.h"

namespace demo_game
{
	using namespace vog;
	class Boundary : public NativeScriptEntity
	{
	public:

	public:
		Boundary() = default;
		virtual ~Boundary() = default;

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
			if (!other.hasComponent<NativeScriptComponent>())
				return;

			RhythmCube* pCube = other.getComponent<NativeScriptComponent>().get<RhythmCube>();
			if (!pCube)
				return;

			auto& cubeTransform = pCube->getComponent<TransformComponent>();
			
			RhythmCubeManager::get().destroyRythm(other);
		};

		virtual void onTriggerStay(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
		};

		virtual void onTriggerExit(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
		};
	};
}