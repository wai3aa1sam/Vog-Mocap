#include "ScoreManager.h"

namespace demo_game {

	ScoreManager* ScoreManager::s_pInstance = nullptr;
	
	void ScoreManager::onAwake()
	{
		VOG_CORE_LOG_INFO("ScoreManager: onAwake!");

		s_pInstance = this;
	}
}