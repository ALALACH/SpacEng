#pragma once
#include "Codes.h"
#include <glfw/include/GLFW/glfw3.h>


namespace Spaceng {
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved
	};
#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }

	class Event
	{
	public:
		bool Handeled = false;
		virtual EventType GetEventType() const = 0;
	};


	class EventDispatcher
	{
		template<typename Fn>
		using EventFn = std::function<bool(Fn&)>;
	public:
		EventDispatcher(Event& event)
			:m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> function)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{ 
			m_Event.Handeled = function(*(T*)&m_Event);
			return true;
			}
			return false;
		}
		
	private:
		Event& m_Event;
	};


	// Window Events

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent(GLFWwindow* Handle)
		:ID_Window(Handle) {}

		inline GLFWwindow* GetWindowID() { return ID_Window; }
		EVENT_CLASS_TYPE(WindowClose)
	private:
		GLFWwindow* ID_Window;
	};


	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t Width, uint32_t Height)
			:m_Width(Width), m_Height(Height) {}

		inline uint32_t GetHeight() const { return m_Height; }
		inline uint32_t GetWidth() const { return m_Width; }
		EVENT_CLASS_TYPE(WindowResize)

	private:
		uint32_t m_Width, m_Height;
	};



	// Keyboard Events
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(Code keycode, int repeatCount)
			: m_KeyCode(keycode), m_RepeatCount(repeatCount) {}

		inline Code GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_TYPE(KeyPressed)
	private:

		Code m_KeyCode;
		uint32_t m_RepeatCount;
	};



	class keyReleasedEvent : public Event
	{
	public:
		keyReleasedEvent(Code keycode)
			:m_KeyCode(keycode){}

		inline Code GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_TYPE(KeyReleased)
	private:

		Code m_KeyCode;
	};





	// Mouse Events
	class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(Code mousebutton)
			:m_MouseButton(mousebutton){}

		inline Code GetMouseButton() const { return m_MouseButton; }
		EVENT_CLASS_TYPE(MouseButtonPressed)
	private:
		Code m_MouseButton;
	};


	class MouseButtonReleasedEvent : public Event
	{
	public:
		MouseButtonReleasedEvent(Code mousebutton)
			:m_MouseButton(mousebutton) {}
		EVENT_CLASS_TYPE(MouseButtonReleased)
	private:
		Code m_MouseButton;
	};


	class MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float XOffset , float YOffset)
			:m_XOffset(XOffset), m_YOffset(YOffset){}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }
		EVENT_CLASS_TYPE(MouseScrolled)
	private:
		float m_XOffset, m_YOffset;
	};


	class MouseMovedEvent :public Event
	{
	public:
		MouseMovedEvent(float XPOS, float YPOS)
			:m_XPos(XPOS), m_YPos(YPOS) {}

		inline float GetXPosition() const { return m_XPos; }
		inline float GetYPosition() const { return m_YPos; }
		EVENT_CLASS_TYPE(MouseMoved)
	private:
		float m_XPos, m_YPos;

	};
}
