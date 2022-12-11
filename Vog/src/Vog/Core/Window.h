#pragma once

#include "vogpch.h"

#include "Vog/Core/Core.h"
#include "Vog/Core/KeyCodes.h"

#include "Vog/Events/Event.h"

#include "Vog/Utilities/MyCommon.h"

namespace vog {

	struct WindowBasicData
	{
	public:
		std::string name = "Vog Engine";
		uint32_t width, height = 0;

		char mouseButtons[static_cast<int>(KeyCode::XButton2) + 1];
		char keys[static_cast<int>(KeyCode::Last_Key)];
		std::pair<float, float> mousePosition = { 0.0f, 0.0f };

		//std::function<void(Window*, uint32_t, uint32_t)> onResizeCallback;

		WindowBasicData(const std::string& name_ = "Vog Engine", uint32_t width_ = 1280, uint32_t height_ = 720)
			:
			name(name_), width(width_), height(height_)
		{}
	};

	class VOG_API Window : public NonCopyable
	{
	public:
		using AppOnEventCallBackFn = std::function<void(Event&)>;
		Window() = default;
		virtual ~Window() = default;

		virtual void onUpdate() = 0;
		virtual void setAppOnEventCallBackFn(AppOnEventCallBackFn fn_) = 0;

		virtual inline uint32_t getWidth() const = 0;
		virtual inline uint32_t getHeight() const = 0;

		virtual void setVsync(bool isVsync_) = 0;
		virtual inline bool isVsync() const = 0;

		virtual inline void* getWindowHandle() const = 0;
		virtual inline void* getGraphicsContext() const = 0;

		static ScopePtr<Window> create(const WindowBasicData& windowBasicData_ = WindowBasicData());

	protected:

	};
}