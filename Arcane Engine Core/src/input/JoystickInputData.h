/*
	Author: Mohamed Kazma
	Descriptions: Holds joystick data and handles events sent from the joystick manager via glfw
	and updates the corresponding data it is holding
*/

#pragma once

#define NUM_JOYSTICK_BUTTONS 16
#define MAX_JOYSTICK_BUTTONS 18

namespace arcane
{
	class JoystickInputData
	{
	public:
		// Axis positions of the left and right analog sticks
		glm::vec2 m_LeftStick;
		glm::vec2 m_RightStick;
		// Values of the left and right triggers
		glm::vec2 m_Triggers;
		// Store the state of each joytick button to be queried
		unsigned char m_ButtonStates[MAX_JOYSTICK_BUTTONS];

		JoystickInputData(char id = 0);
		~JoystickInputData();

		// Update the joystick's connection status
		inline void SetConnection(bool isConnected) { m_Connected = isConnected; }
		// Set the joystick id that we want to query
		inline void SetId(char id) { m_Id = id; }
		// Get the number of buttons that this joystick has
		inline char GetNumButtons() { return m_NumButtons; }
		// Get the left stick values
		inline glm::vec2& GetLeftStick() { return m_LeftStick; }
		// Get Right stick values
		inline glm::vec2& GetRightStick() { return m_RightStick; }
		// Get Triggers
		inline glm::vec2& GetTriggers() { return m_Triggers; }
		// Check if the joystick is connected
		inline bool IsConnected() { return m_Connected; }

		// Update the joystick values by querying for button states
		void Update();

	private:
		// Is the joystick connected
		bool m_Connected; 
		// The id that we will use to query for button states
		char m_Id; 
		// The number of buttons is different between controllers
		int m_NumButtons; 
	};
}
