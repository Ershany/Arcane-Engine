#pragma once

#include "pch.h"

/*
	Events in Arcane are not being buffered, instead they are blocking
	That means every time an event gets dispatched it must be processed
	TODO: Buffer Events and process during the event part of the update stage
*/

namespace arcane
{
	enum class EventType
	{
		None = 0,
		ApplicationUpdate, ApplicationRender, ApplicationTick,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved,
		KeyPressed, KeyReleased, KeyTyped
	};

	enum class EventCategoryFlags : uint8_t
	{
		None = 0,
		EventFlagsApplication = BIT(0),
		EventFlagsInput = BIT(1),
		EventFlagsMouse = BIT(2),
		EventFlagsMouseButton = BIT(3),
		EventFlagsKeyboard = BIT(4)
	};

#define EVENT_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CATEGORY_FLAGS(category) virtual int GetCategoryEventFlags() const override { return category; }

	class Event
	{
	public:
		virtual EventType GetEventType() const = 0;
		virtual uint8_t GetCategoryEventFlags() const = 0;
		virtual const char* GetName() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategoryFlags categoryFlags)
		{
			return GetCategoryEventFlags() & static_cast<uint8_t>(categoryFlags);
		}
	public:
		bool Handled = false; // This will allow certain layers to handle the event and stop the event from further propagating
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool>(T&);
	public:
		EventDispatcher(Event &event) : m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}
	private:
		Event &m_Event;
	};

	inline std::ostream& operator<<(std::ostream &os, const Event &e)
	{
		return os << e.ToString();
	}
}
