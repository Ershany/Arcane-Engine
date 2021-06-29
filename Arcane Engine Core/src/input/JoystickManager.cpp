#include "pch.h"
#include "JoystickManager.h"

namespace arcane {

	JoystickInputData JoystickManager::s_JoystickData[MAX_JOYSTICKS];

	JoystickManager::JoystickManager() {
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			s_JoystickData[i].setId(i);
			s_JoystickData[i].setConnection(glfwJoystickPresent(i)); // Check for joysticks that are already connected
		}
	}

	JoystickManager::~JoystickManager() {}

	void JoystickManager::update() {
		for (int i = 0; i < MAX_JOYSTICKS; ++i) {
			if (!s_JoystickData[i].isConnected())
				continue;

			// Update each joystick that is connected
			s_JoystickData[i].update();
		}
	}

	void JoystickManager::joystickConnectionCallback(int joystick, int event) {
#if ARC_DEBUG
		if (joystick >= MAX_JOYSTICKS)
		{
			ARC_LOG_WARN("Too many joysticks connected, not tracking new connection");
			return;
		}
#endif
		if (event == GLFW_CONNECTED) {
			// TODO: Maybe get controller name and store for more debugging of controller information
			s_JoystickData[joystick].setConnection(true);
			ARC_LOG_TRACE("Joystick {0} has connected successfully", joystick);
		}
		else if (event == GLFW_DISCONNECTED) {
			s_JoystickData[joystick].setConnection(false);
			ARC_LOG_TRACE("Joystick {0} has disconnected successfully", joystick);
		}
	}

	JoystickInputData* JoystickManager::getJoystickInfo(int joystick) {
#if ARC_DEBUG
		if (joystick < 0 || joystick >= MAX_JOYSTICKS)
		{
			ARC_LOG_WARN("Joystick id {0} requested but not loaded", joystick);
			return nullptr;
		}
#endif
		return &s_JoystickData[joystick];
	}

	bool JoystickManager::getButton(int joystickId, int buttonCode) {
		JoystickInputData* controller = getJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= controller->getNumButtons())
			return false;

		return controller->m_ButtonStates[buttonCode] != GLFW_RELEASE;
	}

	bool JoystickManager::getButtonDown(int joystickId, int buttonCode) {
		JoystickInputData* controller = getJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= controller->getNumButtons())
			return false;
		
		// Check that a certain button is pressed and wasn't pressed before to get the first frame that was pressed 
		return controller->m_ButtonStates[buttonCode] == GLFW_PRESS;
	}
}
