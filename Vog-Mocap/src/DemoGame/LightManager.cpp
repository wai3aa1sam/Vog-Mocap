#include "LightManager.h"

namespace demo_game
{
	LightManager* LightManager::s_pInstance = nullptr;

	void LightManager::onAwake()
	{
		VOG_CORE_LOG_INFO("LightManager: onAwake!");

		s_pInstance = this;
	};
}