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
		if (joystick >= MAX_JOYSTICKS) {
			Logger::getInstance().error("logged_files/input_errors.txt", "Joystick Check", "Too many Joysticks connected");
			return;
		}

		if (event == GLFW_CONNECTED) {
			// TODO: Maybe get controller name and store for more debugging of controller information
			s_JoystickData[joystick].setConnection(true);
			std::cout << "joystick " << joystick << "has connected successfully" << std::endl;
		}
		else if (event == GLFW_DISCONNECTED) {
			s_JoystickData[joystick].setConnection(false);
			std::cout << "joystick " << joystick << "has disconnected successfully" << std::endl;
		}
	}

	JoystickInputData* JoystickManager::getJoystickInfo(int joystick) {
		if (joystick < 0 || joystick >= MAX_JOYSTICKS) {
			Logger::getInstance().error("logged_files/input_errors.txt", "Joystick Check", "Joystick data requested does not exist");
			return nullptr;
		}

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
