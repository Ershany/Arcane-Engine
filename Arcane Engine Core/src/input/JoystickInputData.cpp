#include "pch.h"
#include "JoystickInputData.h"

namespace arcane
{
	JoystickInputData::JoystickInputData(char id) : m_Id(id), m_Connected(false)
	{}

	JoystickInputData::~JoystickInputData()
	{}

	void JoystickInputData::Update()
	{
		// Get axis positions (might want to store this a little better)
		int count;
		const float* axes = glfwGetJoystickAxes(m_Id, &count);
		if (count < 6)
		{
			// something is wrong
			return;
		}

		m_LeftStickHorizontal = axes[0];
		m_LeftStickVertical = axes[1];
		m_RightStickHorizontal = axes[2];
		m_RightStickVertical = axes[3];
		m_LeftTrigger = axes[4] * 0.5f + 0.5f;
		m_RightTrigger = axes[5] * 0.5f + 0.5f;

		std::cout << std::endl;
		std::cout << std::endl;
		for (int w = 0; w < count; ++w)
		{
			std::cout << axes[w] << std::endl;
		}
		std::cout << std::endl;
		std::cout << std::endl;

		// Get button states on joystick
		const unsigned char* states = glfwGetJoystickButtons(m_Id, &m_NumButtons);
		m_ButtonStates = (unsigned char*)states;

		std::cout << std::endl;
		std::cout << std::endl;
		for (int w = 0; w < m_NumButtons; ++w)
		{
			std::cout << w << ": " << +m_ButtonStates << std::endl;
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}
}
