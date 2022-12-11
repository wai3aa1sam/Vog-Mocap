#include "RhythmCubeManager.h"

namespace demo_game
{
	RhythmCubeManager* RhythmCubeManager::s_pInstance;

	void RhythmCubeManager::onAwake()
	{
		VOG_CORE_LOG_INFO("RhythmCubeManager: onAwake!");

		s_pInstance = this;
	};
}