#pragma once

#define MAX_JOYSTICKS 16
#define INPUT_LOGGER_DIRECTORY "logged_files/input_errors.txt"

#include <input/JoystickInputData.h>

namespace arcane {

	class JoystickManager {
	public:
		JoystickManager();
		~JoystickManager();
	
		static JoystickInputData* getJoystickInfo(int joystick);

		void joystickConnectionCallback(int joystick, int event);
		void Update();		
		
		static inline bool IsConnected(int joystickId) { return s_JoystickData[joystickId].IsConnected(); }
		
		// Getters
		static bool GetButton(int joystickId, int buttonCode);
		static bool GetButtonDown(int joystickId, int buttonCode);
		static inline glm::vec2& GetLeftStick(int joystickId) { return s_JoystickData[joystickId].GetLeftStick(); }
		static inline glm::vec2& GetRightStick(int joystickId) { return s_JoystickData[joystickId].GetRightStick(); }
		static inline glm::vec2& GetTriggers(int joystickId) { return s_JoystickData[joystickId].GetTriggers(); }

	private:
		static JoystickInputData s_JoystickData[MAX_JOYSTICKS];
	};
}
