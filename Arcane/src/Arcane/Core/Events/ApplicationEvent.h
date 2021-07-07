#pragma once

#include <Arcane/Core/Events/Event.h>

#include <sstream>

namespace Arcane
{
	class ApplicationUpdateEvent : public Event
	{
	public:
		ApplicationUpdateEvent() = default;

		EVENT_TYPE(EventType::ApplicationUpdate);
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::EventFlagsApplication);

		std::string ToString() const override
		{
			return std::string("ApplicationUpdateEvent");
		}
	};

	class ApplicationTickEvent : public Event
	{
	public:
		ApplicationTickEvent() = default;

		EVENT_TYPE(EventType::ApplicationTick);
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::EventFlagsApplication);

		std::string ToString() const override
		{
			return std::string("ApplicationTickEvent");
		}
	};

	class ApplicationRenderEvent : public Event
	{
	public:
		ApplicationRenderEvent() = default;

		EVENT_TYPE(EventType::ApplicationRender);
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::EventFlagsApplication);

		std::string ToString() const override
		{
			return std::string("ApplicationRenderEvent");
		}
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

		EVENT_TYPE(EventType::WindowResize)
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::EventFlagsApplication)

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent - Width:" << m_Width << ", Height:" << m_Height;
			return ss.str();
		}
	private:
		uint32_t m_Width, m_Height;
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_TYPE(EventType::WindowClose);
		EVENT_CATEGORY_FLAGS(EventCategoryFlags::EventFlagsApplication);

		std::string ToString() const override
		{
			return std::string("WindowCloseEvent");
		}
	};
}
