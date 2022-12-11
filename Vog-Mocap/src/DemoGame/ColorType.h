#pragma once
#include <Vog.h>

namespace demo_game
{
	using namespace vog;
	class ColorType : public NativeScriptEntity
	{
	public:
		ColorType() = default;
		virtual ~ColorType() = default;

		enum class Color { Red, Blue };

		static constexpr Vector4f red_saber_albedo_color	= { 1.0f, 1.0f, 1.0f, 1.0f };
		static constexpr Vector4f red_saber_emissive_color	= { 1200.0f, 20.0f, 20.0f, 1.0f };
		static constexpr Vector4f blue_saber_albedo_color	= { 1.0f, 20.0f, 1.0f, 1.0f };
		static constexpr Vector4f blue_saber_emissive_color = { 1.0f, 20.0f, 1200.0f, 1.0f };
		static constexpr Vector4f red_bar_albedo_color		= { 1.0f, 1.0f, 1.0f, 1.0f };
		static constexpr Vector4f red_bar_emissive_color	= { 500.0f, 20.0f, 20.0f, 1.0f };
		static constexpr Vector4f blue_bar_albedo_color		= { 1.0f, 50.0f, 1200.0f, 1.0f };
		static constexpr Vector4f blue_bar_emissive_color	= { 1.0f, 20.0f, 1200.0f, 1.0f };

		static constexpr Vector4f blue_plane_albedo_color = { 1.0f, 10.0f, 30.0f, 1.0f };
		static constexpr Vector4f red_plane_albedo_color = { 40.0f, 1.0f, 1.0f, 1.0f };


		//static ColorType&

	private:
		static ColorType* s_pInstance;
	};
}