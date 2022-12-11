#pragma once

#include "vogpch.h"

#include "Vog/Core/Core.h"
#include "Vog/Events/Event.h"

namespace vog {

	class VOG_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int wdith_, unsigned int height_)
			:
			m_wdith(wdith_), m_height(height_)
		{}
		virtual ~WindowResizeEvent() {};
		inline unsigned int getWidth() const { return m_wdith; }
		inline unsigned int getHeight() const { return m_height; }
		inline std::pair<unsigned int, unsigned int> getWidthHeight() const { return { m_wdith, m_height }; }

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << "wdith: " << m_wdith << ", height:" << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(WindowResize)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryApplication)
	private:
		unsigned int m_wdith, m_height;
	};

	class VOG_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}
		virtual ~WindowCloseEvent() {};

		EVENT_CLASS_TYPE_INIT(WindowClose)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryApplication)
	};

	class VOG_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() {}
		virtual ~AppUpdateEvent() {};

		EVENT_CLASS_TYPE_INIT(AppUpdate)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryApplication)
	};

	class VOG_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() {}
		virtual ~AppTickEvent() {};

		EVENT_CLASS_TYPE_INIT(AppTick)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryApplication)
	};

	class VOG_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() {}
		virtual ~AppRenderEvent() {};

		EVENT_CLASS_TYPE_INIT(AppRender)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryApplication)
	};
}