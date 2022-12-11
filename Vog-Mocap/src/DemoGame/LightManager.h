#pragma once
#include <Vog.h>

#include "RhythmCube.h"

namespace demo_game
{
	using namespace vog;
	class LightManager : public NativeScriptEntity
	{
	public:
		static constexpr int s_max_count = 20;
		std::vector<uint32_t>* m_pLightHandles;

		float m_timer = 0.0f;
		float m_threshold_to_change = 10.0f;
		float m_multiplier = 1.0f;

	public:
		LightManager()
		{
			VOG_ASSERT(!s_pInstance, "");
		}
		virtual ~LightManager()
		{
			s_pInstance = nullptr;
		}

		static LightManager& get() { VOG_ASSERT(s_pInstance, ""); return *s_pInstance; }

		virtual void onAwake() override;

		virtual void onStart() override
		{
			//VOG_CORE_LOG_INFO("Move Entity: onStart!");
			//m_lightHandles.add
			m_pLightHandles = (std::vector<uint32_t>*)getComponent<NativeScriptComponent>().pUeserData;

			/*for (size_t i = 2; i < m_pLightHandles->size(); i++)
			{
				Entity entity = { (*m_pLightHandles)[i], m_entity };
				auto* pLight_Params = &entity.getComponent<LightComponent::Params>().params;
				float inner_angle = 10.0f;
				pLight_Params->z = (MyMath::radians(inner_angle));
				pLight_Params->w = (MyMath::radians(MyRandom::Float(inner_angle + 10.0f, inner_angle + 40.0f)));
			}*/
		};

		virtual void onDestroy() override
		{

		};

		virtual void onUpdate(float dt_) override
		{
			for (size_t i = 2; i < m_pLightHandles->size(); i++)
			{
				Entity entity = { (*m_pLightHandles)[i], m_entity };
				auto& transform = entity.getComponent<TransformComponent>();
				float angle = MyMath::radians(MyRandom::Float(10.0f, 50.0f));
				float speed = MyRandom::Float(1.0f, 5.0f);
				transform.rotation.z += angle * m_multiplier * dt_ * speed;
				transform.rotation.y += angle * -m_multiplier * dt_ * speed;
				transform.rotation.x += angle * -m_multiplier * dt_ * speed;
			}
			if (m_timer > m_threshold_to_change)
			{
				m_multiplier *= -1.0f;
				m_timer = 0.0f;
			}

			m_timer += dt_;
		};

		void setColor(ColorType::Color type_)
		{
			// [0] is mesh
			Entity entity = { (*m_pLightHandles)[0], m_entity };
			if (type_ == ColorType::Color::Blue)
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::blue_plane_albedo_color);
			else
				entity.getComponent<MeshRendererComponent>().pMaterial->setFloat4("u_property.albedo", demo_game::ColorType::red_plane_albedo_color);

		}

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
		static LightManager* s_pInstance;
	};
}