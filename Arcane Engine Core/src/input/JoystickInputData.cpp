#include "pch.h"
#include "JoystickInputData.h"


JoystickInputData::JoystickInputData()
{
	// Set the values of the button states to be false
	for (int i = 0; i < NUM_JOYSTICK_BUTTONS; ++i)
	{
		m_ButtonStates[i] = false;
	}
}


JoystickInputData::~JoystickInputData()
{}
