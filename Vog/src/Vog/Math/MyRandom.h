#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"
#include "Vog/Math/MyMath.h"


#include <random>

namespace vog {

	class VOG_API MyRandom : public NonCopyable
	{
	public:
		static MyRandom& get()
		{
			static MyRandom instance;
			return instance;
		}

		static int Int(int min, int max) { return get()._int(min, max); }				// inclusive
		static float Float(float min, float max) { return get()._float(min, max); }		// inclusive

		static Vector2f Vec2f(float min, float max)
		{
			auto& rmd = get();
			return Vector2f(rmd.Float(min, max), rmd.Float(min, max));
		}
		static Vector3f Vec3f(float min, float max)
		{
			auto& rmd = get();
			return Vector3f(rmd.Float(min, max), rmd.Float(min, max), rmd.Float(min, max));
		}
		
		static Vector4f Vec4f(float min, float max)
		{
			auto& rmd = get();
			return Vector4f(rmd.Float(min, max), rmd.Float(min, max), rmd.Float(min, max), rmd.Float(min, max));
		}

	private:
		MyRandom()
			:
			m_rng(m_rd())
		{

		}
		int _int(int min, int max)
		{
			std::uniform_int_distribution<int> i(min, max);
			return i(m_rng);
		}
		float _float(float min, float max)
		{
			std::uniform_real_distribution<float> f(min, max);
			return f(m_rng);
		}
	private:
		//static MyRandom m_instance;
		std::random_device m_rd;
		std::mt19937 m_rng;
	};
}

//MyRandom MyRandom::m_instance;