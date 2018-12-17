#pragma once

namespace arcane {
	
#define MAX_JOYSTICKS 4

	struct JoystickInputData {
		bool isJoystickConnected = false;

		// Range [-1, 1]
		float leftStickHorizontal = 0, leftStickVertical = 0;
		float rightStickHorizontal = 0, rightStickVertical = 0;

		// Range [0, 1]
		float leftTrigger = 0, rightTrigger = 0;
	};

	class JoystickManager {
	public:
		JoystickManager();
		~JoystickManager();

		void Update();
		void JoystickConnectionCallback(int joystick, int event);
	private:
		static JoystickInputData s_JoystickData[MAX_JOYSTICKS];
	};

}
