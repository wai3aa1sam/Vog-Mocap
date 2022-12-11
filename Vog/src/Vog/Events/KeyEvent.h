#pragma once

#include "vogpch.h"

#include "Vog/Core/Core.h"
#include "Vog/Events/Event.h"

namespace vog {

	class VOG_API KeyEvent : public Event
	{
	public:
		KeyEvent(int keycode_)
			:
			m_keyCode(keycode_)
		{}
		virtual ~KeyEvent() {};
		inline unsigned int getKeyCode() const { return m_keyCode; }

		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryInput | EventCategory::EventCategoryKey)
	protected:
		int m_keyCode;
	};

	class VOG_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode_, int repeatCount_)
			:
			KeyEvent(keycode_), m_repeatCount(repeatCount_)
		{}
		virtual ~KeyPressedEvent() override {}

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << ", repeat: " << m_repeatCount;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(KeyPressed)
	private:
		int m_repeatCount;
	};

	class VOG_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode_)
			:
			KeyEvent(keycode_)
		{}
		virtual ~KeyReleasedEvent() override {}

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(KeyReleased)
	};

	class VOG_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode_)
			:
			KeyEvent(keycode_)
		{}
		virtual ~KeyTypedEvent() override {}

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << static_cast<char>(m_keyCode);
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(KeyTyped)
	};
}