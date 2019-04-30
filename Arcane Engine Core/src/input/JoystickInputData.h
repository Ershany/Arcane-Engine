/*
	Author: Mohamed Kazma
	Descriptions: Holds joystick data and handles events sent from the joystick manager via glfw
	and updates the corresponding data it i holding
*/

#pragma once

#define NUM_JOYSTICK_BUTTONS 16

namespace arcane
{
	class JoystickInputData
	{
	public:
		// Range [-1, 1]
		float m_LeftStickHorizontal;
		float m_LeftStickVertical;
		float m_RightStickHorizontal;
		float m_RightStickVertical;

		// Range [0, 1] R1 and l1
		float m_LeftTrigger;
		float m_RightTrigger;

		// Store the state of each joytick button to be queried
		unsigned char* m_ButtonStates;

		JoystickInputData(char id);
		~JoystickInputData();

		inline void SetConnection(bool isConnected) { m_Connected = isConnected; }
		inline bool IsConnected() { return m_Connected; }
		inline char GetNumButtons() { return m_NumButtons; }
		
		void Update();

	private:
		bool m_Connected;
		char m_Id;
		int m_NumButtons; // The number of buttons is different between controllers
	};
}
