#pragma once

#define MAX_JOYSTICK_BUTTONS 18

#define ARCANE_GAMEPAD_A 0
#define ARCANE_GAMEPAD_B 1
#define ARCANE_GAMEPAD_X 2
#define ARCANE_GAMEPAD_Y 3
#define ARCANE_GAMEPAD_LEFT_BUMPER 4
#define ARCANE_GAMEPAD_RIGHT_BUMPER 5
#define ARCANE_GAMEPAD_BACK 6
#define ARCANE_GAMEPAD_START 7
#define ARCANE_GAMEPAD_GUIDE 8
#define ARCANE_GAMEPAD_LEFT_THUMB 9
#define ARCANE_GAMEPAD_RIGHT_THUMB 10
#define ARCANE_GAMEPAD_DPAD_UP 11
#define ARCANE_GAMEPAD_DPAD_RIGHT 12
#define ARCANE_GAMEPAD_DPAD_DOWN 13
#define ARCANE_GAMEPAD_DPAD_LEFT 14
#define ARCANE_GAMEPAD_LAST   ARCANE_GAMEPAD_DPAD_LEFT
#define ARCANE_GAMEPAD_CROSS ARCANE_GAMEPAD_A
#define ARCANE_GAMEPAD_CIRCLE   ARCANE_GAMEPAD_B
#define ARCANE_GAMEPAD_SQUARE   ARCANE_GAMEPAD_X
#define ARCANE_GAMEPAD_TRIANGLE   ARCANE_GAMEPAD_Y

namespace arcane
{
	class JoystickInputData
	{
	public:
		unsigned char m_ButtonStates[MAX_JOYSTICK_BUTTONS];

		JoystickInputData(char id = 0);
		~JoystickInputData();

		inline void SetConnection(bool isConnected) { m_Connected = isConnected; }
		inline void SetId(char id) { m_Id = id; }
		inline void SetDeadzone(float dead) { m_Deadzone = dead; }
		inline char GetNumButtons() { return m_NumButtons; }
		inline glm::vec2& GetLeftStick() { return m_LeftStick; }
		inline glm::vec2& GetRightStick() { return m_RightStick; }
		inline glm::vec2& GetTriggers() { return m_Triggers; }
		inline bool IsConnected() { return m_Connected; }

		void Update();

	private:
		bool m_Connected; 
		char m_Id; 
		int m_NumButtons; 
		float m_Deadzone;
		glm::vec2 m_LeftStick;
		glm::vec2 m_RightStick;
		glm::vec2 m_Triggers;
	};
}
