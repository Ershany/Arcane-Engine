#pragma once

#include <input/JoystickManager.h>

namespace arcane {

#define MAX_KEYS 1024
#define MAX_BUTTONS 32

	class InputManager {
	public:
		InputManager();
		~InputManager();

		void update();

		static bool isKeyPressed(unsigned int keycode);
		static float getKeyPressure(unsigned int keycode);
		static bool isMouseButtonPressed(unsigned int code);
		static bool getButton(int keyCode);
		static bool getButtonDown(int keyCode);

		void keyCallback(int key, int scancode, int action, int mods);
		void mouseButtonCallback(int button, int action, int mods);
		void cursorPositionCallback(double xpos, double ypos);
		void scrollCallback(double xoffset, double yoffset);
		void joystickCallback(int joystick, int event);

		// Getters and setters
		static inline double getMouseX() { return s_MouseX; }
		static inline double getMouseY() { return s_MouseY; }
		static inline double getMouseXDelta() { return s_MouseXDelta; }
		static inline double getMouseYDelta() { return s_MouseYDelta; }
		static inline double getScrollXDelta() { return s_ScrollXDelta; }
		static inline double getScrollYDelta() { return s_ScrollYDelta; }
		static inline void getMousePosition(double &x, double &y) { x = s_MouseX; y = s_MouseY; }

		static inline void setMouseX(double value) { s_MouseX = value; }
		static inline void setMouseY(double value) { s_MouseY = value; }
		static inline void setMousePos(double xPos, double yPos) { s_MouseX = xPos, s_MouseY = yPos; }
	private:
		JoystickManager m_JoystickManager;

		static bool s_Keys[MAX_KEYS];
		static float s_KeyPressure[MAX_KEYS];
		static bool s_Buttons[MAX_BUTTONS];
		static double s_MouseX, s_MouseY, s_MouseXDelta, s_MouseYDelta;
		static double s_ScrollXDelta, s_ScrollYDelta;
	};

}
