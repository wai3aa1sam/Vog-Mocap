#pragma once
#include <Vog.h>

#include "ColorType.h"
#include "RhythmCube.h"

namespace demo_game
{
	using namespace vog;
	class RhythmCubeManager : public NativeScriptEntity
	{
	public:
		RhythmCubeManager()
		{
			VOG_ASSERT(!s_pInstance, "")
		}
		virtual ~RhythmCubeManager()
		{
			s_pInstance = nullptr;
		}

		Vector3f m_position = { 0.0f, 0.0f, 0.0f };

		RefPtr<Texture2D> m_pRhythm_emission_map;

		std::vector<uint32_t>* m_pCubeHandlePool;
		std::array<Vector3f, 4> m_spawnPoints;

		int m_current_RhythmCount = 0;
		float m_timer = 0;
		float m_beat = 60.0f * 2.0f / 105.0f / 20.0f;

		float m_speed = 20.0f;

		static constexpr int s_max_rhythmCube = 400;

		virtual void onAwake() override;

		virtual void onStart() override
		{
			m_pRhythm_emission_map = Texture2D::create("assets/textures/demo_game/hit_rhythm_emission_map.png");

			m_pCubeHandlePool = (std::vector<uint32_t>*)getComponent<NativeScriptComponent>().pUeserData;
			//createRythm(ColorType::Color::Red);
			m_position = getComponent<TransformComponent>().translation;

			float start = 5.5f;
			float speration = 3.0f;

			m_spawnPoints[0] = m_position;
			m_spawnPoints[0].x -= start;

			m_spawnPoints[1] = m_position;
			m_spawnPoints[1].x -= start - speration;

			m_spawnPoints[2] = m_position;
			m_spawnPoints[2].x += start - speration;

			m_spawnPoints[3] = m_position;
			m_spawnPoints[3].x += start;
		};
		virtual void onDestroy() override
		{
			VOG_CORE_LOG_INFO("RhythmCubeManager: onDestroy!");
		};

		virtual void onUpdate(float dt_) override
		{
			if (m_timer > m_beat)
			{
				createRythm((ColorType::Color)MyRandom::get().Int(0, 1));
				m_timer = 0.0f;
			}

			m_timer += dt_;
		};

		void createRythm(ColorType::Color colorType_)
		{
			if (m_current_RhythmCount >= s_max_rhythmCube)
				return;
			if (m_createIndex >= s_max_rhythmCube)
				m_createIndex = 0;

			Entity entity = Entity{ (*m_pCubeHandlePool)[m_createIndex], m_entity};
			auto& transform = entity.getComponent<TransformComponent>();
			transform.isEnable = false;

			Vector4f albedo = { 0.5f, 0.1f, 0.1f, 1.0f };
			Vector4f emission = { 100.0, 5.0f, 5.0f, 1.0f };

			if (colorType_ == ColorType::Color::Blue)
			{
				albedo = { 0.0f, 0.1f, 0.9f, 1.0f };
				emission = { 1.0, 20.0f, 100.0f, 1.0f };
			}

			entity.getComponent<MeshRendererComponent>().pMaterial->setTexture2D("u_emission_map", m_pRhythm_emission_map);
			entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", albedo);
			entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.emission", emission);
			entity.getComponent<MeshRendererComponent>().pMaterial->setFloat("u_property.shiness", 32.0f);

			transform.translation = m_spawnPoints[MyRandom::get().Int(0, (int)m_spawnPoints.size() - 1)];
			transform.rotation = Vector3f(0.0f, 0.0f, 1.0f) * 90.0f * (float)MyRandom::get().Int(0, 4);

			auto& rigidbody = entity.getComponent<RigidbodyComponent>();
			rigidbody.velocity.z = m_speed;

			RhythmCube* cube = (RhythmCube*)entity.getComponent<NativeScriptComponent>().pInstance;
			cube->m_color = colorType_;

			transform.isEnable = true;

			m_current_RhythmCount++;
			m_createIndex++;
			//VOG_LOG_INFO("Created Rhythm, count: {0}, Handle: {1}", m_current_RhythmCount, (uint32_t)entity);
		}

		void destroyRythm(uint32_t handle_)
		{
			VOG_ASSERT(m_current_RhythmCount > 0, "");
			if (m_current_RhythmCount < 0)
				return;

			bool isFound = false;

			for (size_t i = 0; i < s_max_rhythmCube; i++)			// slow
			{
				if ((*m_pCubeHandlePool)[i] != handle_)
					continue;

				auto toBeRemoveHandle = (*m_pCubeHandlePool)[i];

				Entity entity = Entity{ toBeRemoveHandle, m_entity };
				auto& transform = entity.getComponent<TransformComponent>();
				transform.isEnable = false;
				transform.translation = m_position;

				isFound = true;

				(*m_pCubeHandlePool)[i] = (*m_pCubeHandlePool).back();
				(*m_pCubeHandlePool).back() = toBeRemoveHandle;

				//VOG_LOG_INFO("toBeRemoveHandle: {0}, (*m_pCubeHandlePool)[i]: {1}, (*m_pCubeHandlePool).back(): {2}", toBeRemoveHandle, (*m_pCubeHandlePool)[i], (*m_pCubeHandlePool).back());
				//VOG_LOG_INFO("Destroyed Rhythm, count: {0}, Handle: {1}", m_current_RhythmCount - 1, toBeRemoveHandle);

				break;
			}

			VOG_ASSERT(isFound, "");

			m_current_RhythmCount--;
		}

		static RhythmCubeManager& get() { VOG_ASSERT(s_pInstance, "");  return *s_pInstance; };

	private:
		static RhythmCubeManager* s_pInstance;
		int m_createIndex = 0;
	};
}