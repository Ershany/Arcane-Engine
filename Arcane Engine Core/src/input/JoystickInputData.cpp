#include "pch.h"
#include "JoystickInputData.h"

namespace arcane
{
	JoystickInputData::JoystickInputData(char id) : m_Id(id), m_Connected(false), m_Deadzone(0.005f)
	{}

	JoystickInputData::~JoystickInputData()
	{}

	void JoystickInputData::update()
	{
		// Get axis positions (might want to store this a little better glm::vec2s)
		int count;
		const float* axes = glfwGetJoystickAxes(m_Id, &count);
		if (count < 6)
			return;

		// Deadzone can be checked here 
		m_LeftStick.x = axes[0];
		m_LeftStick.y = axes[1];
		m_RightStick.x = axes[2];
		m_RightStick.y = axes[3];
		m_Triggers.x = axes[4] * 0.5f + 0.5f;
		m_Triggers.y = axes[5] * 0.5f + 0.5f;
		
		//for (int w = 0; w < count; ++w)
		//{
		//	std::cout << axes[w] << std::endl;
		//}

		// Get button states on joystick
		const unsigned char* states = glfwGetJoystickButtons(m_Id, &m_NumButtons);
		for (int w = 0; w < m_NumButtons; ++w)
		{
			if (m_ButtonStates[w] != GLFW_RELEASE && states[w] == GLFW_PRESS)
				m_ButtonStates[w] = GLFW_REPEAT;
			else
				m_ButtonStates[w] = states[w];

			//std::cout << w << ": " << +m_ButtonStates[w] << std::endl;
		}
	}
}
