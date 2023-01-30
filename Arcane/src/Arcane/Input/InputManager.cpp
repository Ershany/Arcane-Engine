#include "arcpch.h"
#include "InputManager.h"

namespace Arcane
{
	// Static declarations
	u8 InputManager::s_Keys[MAX_KEYS];
	std::vector<u16> InputManager::s_FirstPressed;
	float InputManager::s_KeyPressure[MAX_KEYS];
	bool InputManager::s_Buttons[MAX_BUTTONS];
	double InputManager::s_MouseX, InputManager::s_MouseY, InputManager::s_MouseXDelta, InputManager::s_MouseYDelta;
	double InputManager::s_ScrollXDelta, InputManager::s_ScrollYDelta;

	InputManager::InputManager() {
		s_ScrollXDelta = s_ScrollYDelta = 0;
		s_MouseXDelta = s_MouseYDelta = 0;
		
		s_FirstPressed.reserve(MAX_KEYS);

		memset(s_Keys, 0, sizeof(bool) * MAX_KEYS);
		memset(s_KeyPressure, 0, sizeof(float) * MAX_KEYS);
		memset(s_Buttons, 0, sizeof(bool) * MAX_BUTTONS);
	}

	InputManager::~InputManager() {}

	InputManager& InputManager::GetInstance()
	{
		static InputManager manager;
		return manager;
	}

	void InputManager::Update() {
		s_MouseXDelta = s_MouseYDelta = 0;
		s_ScrollXDelta = 0; s_ScrollYDelta = 0;

		// Clear all first presses
		s_FirstPressed.clear();

		m_JoystickManager.Update();
	}

	bool InputManager::IsKeyPressed(unsigned int keycode) {
#ifdef ARC_DEV_BUILD
		if (keycode < 0 || keycode >= MAX_KEYS)
		{
			ARC_LOG_WARN("Key press check is out of bounds (ie not supported) - Keycode: {0}", keycode);
			return false;
		}
#endif // ARC_DEV_BUILD
		return s_Keys[keycode] != GLFW_RELEASE;
	}

	bool InputManager::IsKeyPressedDown(unsigned int keycode) {
#ifdef ARC_DEV_BUILD
		if (keycode < 0 || keycode >= MAX_KEYS)
		{
			ARC_LOG_WARN("Key press check is out of bounds (ie not supported) - Keycode: {0}", keycode);
			return false;
		}
#endif // ARC_DEV_BUILD

		for (int i = 0; i < s_FirstPressed.size(); ++i)
		{
			if (s_FirstPressed[i] == keycode)
				return true;
		}

		return false;
	}

	float InputManager::GetKeyPressure(unsigned int keycode) {
#ifdef ARC_DEV_BUILD
		if (keycode < 0 || keycode >= MAX_KEYS)
		{
			ARC_LOG_WARN("Key pressure get is out of bounds (ie not supported) - Keycode: {0}", keycode);
			return 0.0f;
		}
#endif // ARC_DEV_BUILD
		return s_KeyPressure[keycode];
	}

	bool InputManager::IsMouseButtonPressed(unsigned int code) {
#ifdef ARC_DEV_BUILD
		if (code < 0 || code >= MAX_BUTTONS)
		{
			ARC_LOG_WARN("Mouse button press check is out of bounds (ie not supported) - code: {0}", code);
			return false;
		}
#endif // ARC_DEV_BUILD
		return s_Buttons[code];
	}

	void InputManager::KeyCallback(int key, int scancode, int action, int mods) {
		s_Keys[key] = action;
		s_KeyPressure[key] = max(s_Keys[key], 1);

		if (action == GLFW_PRESS)
		{
			s_FirstPressed.push_back(key);
		}
	}

	void InputManager::MouseButtonCallback(int button, int action, int mods) {
		s_Buttons[button] = action != GLFW_RELEASE;
	}

	void InputManager::CursorPositionCallback(double xpos, double ypos) {
		s_MouseXDelta = xpos - s_MouseX;
		s_MouseYDelta = ypos - s_MouseY;
		s_MouseX = xpos;
		s_MouseY = ypos;
	}

	void InputManager::ScrollCallback(double xoffset, double yoffset) {
		s_ScrollXDelta = xoffset;
		s_ScrollYDelta = yoffset;
	}

	void InputManager::JoystickCallback(int joystick, int event) {
		m_JoystickManager.JoystickConnectionCallback(joystick, event);
	}

	bool InputManager::GetButton(int keyCode) {
#ifdef ARC_DEV_BUILD
		if (keyCode < 0 || keyCode >= MAX_BUTTONS)
		{
			ARC_LOG_WARN("Button get is out of bounds (ie not supported) - KeyCode: {0}", keyCode);
			return false;
		}
#endif // ARC_DEV_BUILD
		return s_Buttons[keyCode] != GLFW_RELEASE;
	}

	bool InputManager::GetButtonDown(int keyCode) {
#ifdef ARC_DEV_BUILD
		if (keyCode < 0 || keyCode >= MAX_BUTTONS)
		{
			ARC_LOG_WARN("Button down get is out of bounds (ie not supported) - KeyCode: {0}", keyCode);
			return false;
		}
#endif // ARC_DEV_BUILD

		return s_Buttons[keyCode] == GLFW_REPEAT;
	}
}
