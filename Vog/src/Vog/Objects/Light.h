#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Math/MyMath.h"

namespace vog {

	enum LightType
	{
		Point			= 0,
		Directional,
		Spot,
		Area,



		Count,
	};

	struct VOG_API Light
	{
	public:
		static constexpr int s_max_light_count = 100;
		static int s_current_light_count;

		//struct Type
		//{
		//	LightType type;
		//	Type() = default;
		//	Type(const Type&) = default;
		//	~Type() = default;
		//};
		//struct Position
		//{
		//	Vector4f posiiton;
		//	/*Position()
		//	{
		//		VOG_CORE_LOG_TRACE("Light position create!");
		//	}*/
		//	Position(const Position&) = default;
		//	~Position() = default;
		//	/*~Position()
		//	{
		//		VOG_CORE_LOG_TRACE("Light position destroy!");
		//	}*/
		//};
		//struct Color
		//{
		//	Vector4f color;
		//	Color() = default;
		//	Color(const Color&) = default;
		//	~Color() = default;
		//};
		//struct Params
		//{
		//	Vector4f params;
		//	Params() = default;
		//	Params(const Params&) = default;
		//	~Params() = default;
		//};

		//Light();
		//~Light();
		//float CalcPointLightBoundSphere(float constant_ = 1.0f, float linear_ = 0.7f, float quadratic_ = 1.8f);

	//public:
	//	//Vector4f position;
	//	Vector4f color;
	//	Vector4f param;
	};
}