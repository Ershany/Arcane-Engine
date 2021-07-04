#include "arcpch.h"
#include "JoystickManager.h"

namespace Arcane
{
	JoystickInputData JoystickManager::s_JoystickData[MAX_JOYSTICKS];

	JoystickManager::JoystickManager() {
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			s_JoystickData[i].SetId(i);
			s_JoystickData[i].SetConnection(glfwJoystickPresent(i)); // Check for joysticks that are already connected
		}
	}

	JoystickManager::~JoystickManager() {}

	void JoystickManager::Update() {
		for (int i = 0; i < MAX_JOYSTICKS; ++i) {
			if (!s_JoystickData[i].IsConnected())
				continue;

			// Update each joystick that is connected
			s_JoystickData[i].Update();
		}
	}

	void JoystickManager::JoystickConnectionCallback(int joystick, int event) {
#ifdef ARC_DEV_BUILD
		if (joystick >= MAX_JOYSTICKS)
		{
			ARC_LOG_WARN("Too many joysticks connected, not tracking new connection");
			return;
		}
#endif // ARC_DEV_BUILD
		if (event == GLFW_CONNECTED) {
			// TODO: Maybe get controller name and store for more debugging of controller information
			s_JoystickData[joystick].SetConnection(true);
			ARC_LOG_TRACE("Joystick {0} has connected successfully", joystick);
		}
		else if (event == GLFW_DISCONNECTED) {
			s_JoystickData[joystick].SetConnection(false);
			ARC_LOG_TRACE("Joystick {0} has disconnected successfully", joystick);
		}
	}

	JoystickInputData* JoystickManager::GetJoystickInfo(int joystick) {
#ifdef ARC_DEV_BUILD
		if (joystick < 0 || joystick >= MAX_JOYSTICKS)
		{
			ARC_LOG_WARN("Joystick id {0} requested but not loaded", joystick);
			return nullptr;
		}
#endif // ARC_DEV_BUILD
		return &s_JoystickData[joystick];
	}

	bool JoystickManager::GetButton(int joystickId, int buttonCode) {
		JoystickInputData* controller = GetJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= controller->GetNumButtons())
			return false;

		return controller->m_ButtonStates[buttonCode] != GLFW_RELEASE;
	}

	bool JoystickManager::GetButtonDown(int joystickId, int buttonCode) {
		JoystickInputData* controller = GetJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= controller->GetNumButtons())
			return false;
		
		// Check that a certain button is pressed and wasn't pressed before to get the first frame that was pressed 
		return controller->m_ButtonStates[buttonCode] == GLFW_PRESS;
	}
}
