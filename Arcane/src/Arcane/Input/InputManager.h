#pragma once

#include <Arcane/Input/JoystickManager.h>

namespace Arcane
{
#define MAX_KEYS 1024
#define MAX_BUTTONS 32

	class InputManager {
	public:
		InputManager();
		~InputManager();

		void Update();

		static bool IsKeyPressed(unsigned int keycode);
		static bool IsKeyPressedDown(unsigned int keycode);
		static float GetKeyPressure(unsigned int keycode);
		static bool IsMouseButtonPressed(unsigned int code);
		static bool GetButton(int keyCode);
		static bool GetButtonDown(int keyCode);

		void KeyCallback(int key, int scancode, int action, int mods);
		void MouseButtonCallback(int button, int action, int mods);
		void CursorPositionCallback(double xpos, double ypos);
		void ScrollCallback(double xoffset, double yoffset);
		void JoystickCallback(int joystick, int event);

		// Getters and setters
		static inline double GetMouseX() { return s_MouseX; }
		static inline double GetMouseY() { return s_MouseY; }
		static inline double GetMouseXDelta() { return s_MouseXDelta; }
		static inline double GetMouseYDelta() { return s_MouseYDelta; }
		static inline double GetScrollXDelta() { return s_ScrollXDelta; }
		static inline double GetScrollYDelta() { return s_ScrollYDelta; }
		static inline void GetMousePosition(double &x, double &y) { x = s_MouseX; y = s_MouseY; }

		static inline void SetMouseX(double value) { s_MouseX = value; }
		static inline void SetMouseY(double value) { s_MouseY = value; }
		static inline void SetMousePos(double xPos, double yPos) { s_MouseX = xPos, s_MouseY = yPos; }
	private:
		JoystickManager m_JoystickManager;

		static u8 s_Keys[MAX_KEYS];
		static float s_KeyPressure[MAX_KEYS];
		static bool s_Buttons[MAX_BUTTONS];
		static double s_MouseX, s_MouseY, s_MouseXDelta, s_MouseYDelta;
		static double s_ScrollXDelta, s_ScrollYDelta;
	};
}
