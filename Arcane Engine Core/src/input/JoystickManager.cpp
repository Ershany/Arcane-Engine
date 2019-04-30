#include "pch.h"
#include "JoystickManager.h"

namespace arcane {

	// Static declarations
	JoystickInputData JoystickManager::s_JoystickData[MAX_JOYSTICKS];

	JoystickManager::JoystickManager() {
		// Setup Joysticks
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			s_JoystickData[i].SetConnection(glfwJoystickPresent(GLFW_JOYSTICK_1 + i)); // This will return 0 or 1 
		}
	}

	JoystickManager::~JoystickManager() {}

	void JoystickManager::update() {
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			if (!s_JoystickData[i].IsConnected())
				continue;

			// Get axis positions (might want to store this a little better)
			int count;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &count);
			if (count != 6)
				continue;

			s_JoystickData[i].m_LeftStickHorizontal = axes[0];
			s_JoystickData[i].m_LeftStickVertical = axes[1];
			s_JoystickData[i].m_RightStickHorizontal = axes[2];
			s_JoystickData[i].m_RightStickVertical = axes[3];
			s_JoystickData[i].m_LeftTrigger = axes[4] * 0.5f + 0.5f;
			s_JoystickData[i].m_RightTrigger = axes[5] * 0.5f + 0.5f;

			// Get button states on joystick
			int buttonCount;
			const unsigned char* states = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &buttonCount);
			s_JoystickData[i].m_ButtonStates = (unsigned char*)states;

			std::cout << "Number of joystick buttons: " << buttonCount << std::endl;
		}
	}

	void JoystickManager::joystickConnectionCallback(int joystick, int event) {
		if (joystick >= MAX_JOYSTICKS) {
			Logger::getInstance().error("logged_files/input_errors.txt", "Joystick Check", "Too many Joysticks connected");
			return;
		}

		if (event == GLFW_CONNECTED) {
			// Check if we have a gamepad
			//if (!glfwJoystickIsGamepad(joystick))
			//	Logger::getInstance().error(INPUT_LOGGER_DIRECTORY, "Gamepad Check", "This joystick is not a gamepad and is not supported");

			s_JoystickData[joystick].SetConnection(true);
		}
		else if (event == GLFW_DISCONNECTED) {
			s_JoystickData[joystick].SetConnection(false);
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
		if (!controller || buttonCode < 0 || buttonCode >= NUM_JOYSTICK_BUTTONS)
			return false;

		return controller->m_ButtonStates[buttonCode] != GLFW_RELEASE;
	}

	bool JoystickManager::GetButtonDown(int joystickId, int buttonCode)
	{
		JoystickInputData* controller = getJoystickInfo(joystickId);
		if (!controller || buttonCode < 0 || buttonCode >= NUM_JOYSTICK_BUTTONS)
			return false;

		// Check if the button has been pressed once and if it has return false for later presses until we get a release call(might need to store previous presses) 
		return controller->m_ButtonStates[buttonCode] == GLFW_PRESS;
	}
}
