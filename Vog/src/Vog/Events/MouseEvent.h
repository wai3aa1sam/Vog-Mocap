#pragma once

#include "vogpch.h"

#include "Vog/Core/Core.h"
#include "Vog/Events/Event.h"

namespace vog {

	class VOG_API MouseButtonEvent : public Event
	{
	public:
		MouseButtonEvent(int button_)
			:
			m_button(button_)
		{}
		virtual ~MouseButtonEvent() {};
		inline unsigned int getButton() const { return m_button; }

		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryInput | EventCategory::EventCategoryMouse)
	protected:
		int m_button;
	};

	class VOG_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(unsigned int button_)
			:
			MouseButtonEvent(button_)
		{}
		virtual ~MouseButtonPressedEvent() override {}

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(MouseButtonPressed)
	};

	class VOG_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(unsigned int button_)
			:
			MouseButtonEvent(button_)
		{}
		virtual ~MouseButtonReleasedEvent() override {}

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(MouseButtonReleased)
	};

	class VOG_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x_, float y_)
			:
			m_mouseX(x_), m_mouseY(y_)
		{}
		virtual ~MouseMovedEvent() {};
		inline float getMouseX() const { return m_mouseX; }
		inline float getMouseY() const { return m_mouseY; }
		inline std::pair<float, float> getMouseXY() const { return { m_mouseX, m_mouseY }; }

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << "x: " << m_mouseX << ", y:" << m_mouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(MouseMoved)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryMouse)
	private:
		float m_mouseX, m_mouseY;
	};

	class VOG_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float x_, float y_)
			:
			m_offsetX(x_), m_offsetY(y_)
		{}
		virtual ~MouseScrolledEvent() {};
		inline float getOffsetX() const { return m_offsetX; }
		inline float getOffsetY() const { return m_offsetY; }
		inline std::pair<float, float> getOffsetXY() const { return { m_offsetX, m_offsetY }; }

		virtual std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << "x: " << m_offsetX << ", y:" << m_offsetY;
			return ss.str();
		}

		EVENT_CLASS_TYPE_INIT(MouseScrolled)
		EVENT_CLASS_CAT_INIT(EventCategory::EventCategoryMouse)
	private:
		float m_offsetX, m_offsetY;
	};
}