#pragma once
#include "vogpch.h"
#include "Vog/Core/Core.h"

namespace vog {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		KeyPressed, KeyReleased, KeyTyped,
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication		= BIT(0),
		EventCategoryInput				= BIT(1),
		EventCategoryKey				= BIT(2),
		EventCategoryMouse				= BIT(3),
		EventCategoryMouseButton		= BIT(4),
	};

#define EVENT_CLASS_TYPE_INIT(type_) \
		virtual inline char* getName() const override { return #type_; } \
		static inline EventType getStaticEventType() { return EventType::##type_; } \
		virtual inline EventType getEventType() const override { return getStaticEventType(); } \
//-------------------------

#define EVENT_CLASS_CAT_INIT(flag_) virtual inline int getCategory() const { return flag_; }
//---------------
	class VOG_API Event
	{
	public:
		virtual ~Event() {};
		virtual inline char* getName() const = 0;
		virtual inline std::string toString() const { return getName(); }
		virtual inline EventType getEventType() const = 0;
		virtual inline int getCategory() const = 0;
		inline bool isInCategory(EventCategory category_) const { return getCategory() & category_; };

	public:
		bool isHandled = false;
	};

	class VOG_API EventDispatcher
	{
		template<typename T>
		using EventCallbackFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event_) 
			:
			m_event(event_)
		{};

		template<typename T>
		bool dispatch(EventCallbackFn<T> fn_)
		{
			if (m_event.getEventType() == T::getStaticEventType())
			{
				bool ret = fn_(static_cast<T&>(m_event));
				m_event.isHandled |= ret;
				return m_event.isHandled;
			}
			return false;
		}

	private:
		Event& m_event;
	};

	inline std::ostream& operator<<(std::ostream& os_, const Event& event_)
	{
		return os_ << event_.toString();
	}
}