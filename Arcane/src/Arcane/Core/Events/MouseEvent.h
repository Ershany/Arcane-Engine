#pragma once
#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#ifndef EVENT_H
#include <Arcane/Core/Events/Event.h>
#endif

namespace Arcane
{
	class MouseButtonEvent : public Event
	{
	protected:
		MouseButtonEvent(int mouseButton) : m_Button(mouseButton) {}
	public:
		EVENT_CATEGORY_FLAGS(static_cast<uint8_t>(EventCategoryFlags::EventFlagsInput) | static_cast<uint8_t>(EventCategoryFlags::EventFlagsMouse) | static_cast<uint8_t>(EventCategoryFlags::EventFlagsMouseButton));

		inline int GetMouseButton() const { return m_Button; }
	protected:
		int m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int mouseButton) : MouseButtonEvent(mouseButton) {}

		EVENT_TYPE(EventType::MouseButtonPressed);

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent - Button:" << m_Button;
			return ss.str();
		}
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int mouseButton) : MouseButtonEvent(mouseButton) {}

		EVENT_TYPE(EventType::MouseButtonReleased);

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent - Button:" << m_Button;
			return ss.str();
		}
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

		EVENT_TYPE(EventType::MouseMoved);
		EVENT_CATEGORY_FLAGS(static_cast<uint8_t>(EventCategoryFlags::EventFlagsInput) | static_cast<uint8_t>(EventCategoryFlags::EventFlagsMouse));

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent - MouseX:" << m_MouseX << ", MouseY:" << m_MouseY;
			return ss.str();
		}
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

		EVENT_TYPE(EventType::MouseScrolled);
		EVENT_CATEGORY_FLAGS(static_cast<uint8_t>(EventCategoryFlags::EventFlagsInput) | static_cast<uint8_t>(EventCategoryFlags::EventFlagsMouse));

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent - XOffset:" << m_XOffset << ", YOffset:" << m_YOffset;
			return ss.str();
		}
	private:
		float m_XOffset, m_YOffset;
	};
}
#endif
