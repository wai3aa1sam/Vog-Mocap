#pragma once
#include <Vog.h>
#include "RhythmCube.h"

#include "RhythmCubeManager.h"

#include "LightManager.h"

namespace demo_game
{
//#define ENABLE_SABER_DEBUG_LOG

	using namespace vog;
	class Saber : public NativeScriptEntity
	{
	public:
		Vector3f prev_position = {0.0f, 0.0f, 0.0f};
		ColorType::Color m_color;

		float sweepSpeed = 5.0f;
		float sweepThreshold_L = -1.0f;
		float sweepThreshold_R = 1.0f;
		bool  shdChangeDirection = false;

		bool* isAnimate = nullptr;

	public:
		Saber() = default;
		virtual ~Saber()
		{
			s_count--;
		}

		virtual void onStart()
		{
			//VOG_CORE_LOG_INFO("Move Entity: onStart!");

			m_color = (ColorType::Color)s_count;
			s_count++;

			isAnimate = reinterpret_cast<bool*>(getComponent<NativeScriptComponent>().pUeserData);
		};

		virtual void onDestroy()
		{
			
		};

		virtual void onUpdate(float dt_)
		{
			auto& transform = getComponent<TransformComponent>();
			prev_position = transform.translation;

			if (*isAnimate)
			{
				if (shdChangeDirection)
				{
					transform.rotation.x += dt_ * sweepSpeed;
					if (transform.rotation.x >= sweepThreshold_R)
					{
						shdChangeDirection = !shdChangeDirection;
					}
				}
				else
				{
					transform.rotation.x -= dt_ * sweepSpeed;
					if (transform.rotation.x <= sweepThreshold_L)
					{
						shdChangeDirection = !shdChangeDirection;
					}
				}
			}
		};

		virtual void onTriggerEnter(uint32_t handle_) override
		{
#ifdef ENABLE_SABER_DEBUG_LOG
			VOG_LOG_INFO("==Start==");

			VOG_LOG_INFO("prev_position");
			VOG_LOG_GLM(prev_position);

			VOG_LOG_INFO("current_position");
			VOG_LOG_GLM(getComponent<TransformComponent>().translation);
#endif // ENABLE_SABER_DEBUG_LOG

			Entity other = { handle_, m_entity };
			if (!other.hasComponent<NativeScriptComponent>())
				return;

			RhythmCube* pCube = other.getComponent<NativeScriptComponent>().get<RhythmCube>();
			if (!pCube)
				return;

			/*if (m_color != pCube->m_color)
			{
				return;
			}*/

			LightManager::get().setColor(m_color);

			auto& saberTransform = getComponent<TransformComponent>();
			auto saber_up = MyMath::toQuaternion(saberTransform.rotation) * Vector3f(0.0f, 1.0f, 0.0f);

			auto& cubeTransform = pCube->getComponent<TransformComponent>();
			auto cube_up = MyMath::toQuaternion(cubeTransform.rotation)* Vector3f(0.0f, 1.0f, 0.0f);

			auto cos_angle = MyMath::dot(saber_up, cube_up);
			if (cos_angle > 0.95f)
			{
				//OG_LOG_INFO("Hitted cube!");
			}

			RhythmCubeManager::get().destroyRythm(other);


			/*auto dir = getComponent<TransformComponent>().translation - prev_position;
			dir = MyMath::normalize(dir);

			auto cos_angle = MyMath::dot(dir, cube_up);
			auto angle = MyMath::acos(cos_angle);

			if (angle > MyMath::radians(130.0f))
			{
			}*/


#ifdef ENABLE_SABER_DEBUG_LOG
			VOG_LOG_INFO("cube_up");
			VOG_LOG_GLM(cube_up);
			VOG_LOG_INFO("angle: {0}", angle);

			if (angle > MyMath::radians(130.0f))
			{
				VOG_LOG_INFO("Saber hitted RhythmCube!");
			}
			VOG_LOG_INFO("==End==");
#endif // ENABLE_SABER_DEBUG_LOG
		};

		virtual void onTriggerStay(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };
			//VOG_CORE_LOG_INFO("Move Entity: onTriggerStay!");
		};

		virtual void onTriggerExit(uint32_t handle_) override
		{
			Entity other = { handle_, m_entity };


#if 0
			const std::string& my_name = m_entity.getComponent<TagComponent>().tag;
			const std::string& other_name = other.getComponent<TagComponent>().tag;

			VOG_CORE_LOG_INFO("{0} with {1}: onTriggerExit!", my_name, other_name);
#endif // 0
		};

		private:
			static int s_count;
	};
}