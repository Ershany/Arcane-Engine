#include "pch.h"
#include "JoystickManager.h"

namespace arcane {

	JoystickManager::JoystickManager() 
	{
		for (int i = 0; i < MAX_JOYSTICKS; ++i)
		{
			s_JoystickData[i] = JoystickInputData(GLFW_JOYSTICK_1 + i); // Store the joystick id 
		}
	}

	JoystickManager::~JoystickManager() {}

	void JoystickManager::update() {
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			if (!s_JoystickData[i].IsConnected())
				continue;

			s_JoystickData[i].Update(); // Update each joystick that is connected
		}
	}

	void JoystickManager::joystickConnectionCallback(int joystick, int event) {
		if (joystick >= MAX_JOYSTICKS) {
			Logger::getInstance().error("logged_files/input_errors.txt", "Joystick Check", "Too many Joysticks connected");
			return;
		}

		if (event == GLFW_CONNECTED) {
			// Maybe get controller name and store for more debugging of controller information
			s_JoystickData[joystick].SetConnection(true);
			std::cout << "joystick " << joystick << "has connected successfully" << std::endl;
		}
		else if (event == GLFW_DISCONNECTED) {
			s_JoystickData[joystick].SetConnection(false);
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

	bool JoystickManager::GetButton(int joystickId, int buttonCode)
	{
		JoystickInputData* controller = getJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= controller->GetNumButtons())
			return false;

		return controller->m_ButtonStates[buttonCode] != GLFW_RELEASE;
	}

	bool JoystickManager::GetButtonDown(int joystickId, int buttonCode)
	{
		JoystickInputData* controller = getJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= controller->GetNumButtons())
			return false;

		// Check if the button has been pressed once and if it has return false for later presses until we get a release call(might need to store previous presses) 
		return controller->m_ButtonStates[buttonCode] == GLFW_PRESS;
	}
}
