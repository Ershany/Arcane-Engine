#include "pch.h"
#include "JoystickInputData.h"

namespace arcane
{
	JoystickInputData::JoystickInputData(char id) : m_Id(id), m_Connected(false), m_Deadzone(0.005f) {
		memset(m_ButtonStates, 0, sizeof(unsigned char) * MAX_JOYSTICK_BUTTONS);
	}

	JoystickInputData::~JoystickInputData() {}

	void JoystickInputData::update() {
		// Get axis positions
		int count;
		const float* axes = glfwGetJoystickAxes(m_Id, &count);
		if (count < 6)
			return;

		// TODO: Deadzone can be checked here 
		m_LeftStick.x = glm::clamp(axes[0], -1.0f, 1.0f);
		m_LeftStick.y = glm::clamp(axes[1], -1.0f, 1.0f);
		m_RightStick.x = glm::clamp(axes[2], -1.0f, 1.0f);
		m_RightStick.y = glm::clamp(axes[3], -1.0f, 1.0f);
		m_Triggers.x = axes[4] * 0.5f + 0.5f;
		m_Triggers.y = axes[5] * 0.5f + 0.5f;

		// Get button states on joystick
		const unsigned char* states = glfwGetJoystickButtons(m_Id, &m_NumButtons);
		for (int w = 0; w < m_NumButtons; w++) {
			if (m_ButtonStates[w] != GLFW_RELEASE && states[w] == GLFW_PRESS)
				m_ButtonStates[w] = GLFW_REPEAT;
			else
				m_ButtonStates[w] = states[w];
		}
	}
}
