#include "pch.h"
#include "JoystickManager.h"

namespace arcane {

	// Static declarations
	JoystickInputData JoystickManager::s_JoystickData[MAX_JOYSTICKS];

	JoystickManager::JoystickManager() {
		// Setup Joysticks
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			s_JoystickData[i].isJoystickConnected = glfwJoystickPresent(GLFW_JOYSTICK_1 + i);
		}
	}

	JoystickManager::~JoystickManager() {}

	void JoystickManager::update() {
		for (int i = 0; i < MAX_JOYSTICKS; i++) {
			if (!s_JoystickData[i].isJoystickConnected)
				continue;

			int count;
			const float *axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &count);
			if (count != 6) {
				continue;
			}

			s_JoystickData[i].leftStickHorizontal = axes[0];
			s_JoystickData[i].leftStickVertical = axes[1];
			s_JoystickData[i].rightStickHorizontal = axes[2];
			s_JoystickData[i].rightStickVertical = axes[3];
			s_JoystickData[i].leftTrigger = axes[4] * 0.5f + 0.5f;
			s_JoystickData[i].rightTrigger = axes[5] * 0.5f + 0.5f;
		}
	}

	void JoystickManager::joystickConnectionCallback(int joystick, int event) {
		if (joystick >= MAX_JOYSTICKS) {
			Logger::getInstance().error("logged_files/input_errors.txt", "Joystick Check", "Too many Joysticks connected");
			return;
		}

		if (event == GLFW_CONNECTED) {
			s_JoystickData[joystick].isJoystickConnected = true;
		}
		else if (event == GLFW_DISCONNECTED) {
			s_JoystickData[joystick].isJoystickConnected = false;
		}
	}

	JoystickInputData *JoystickManager::getJoystickInfo(int joystick) {
		if (joystick >= MAX_JOYSTICKS) {
			Logger::getInstance().error("logged_files/input_errors.txt", "Joystick Check", "Joystick data requested does not exist");
			return nullptr;
		}

		return &s_JoystickData[joystick];
	}

}
