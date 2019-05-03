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
		void update();		
		
		static inline bool IsConnected(int joystickId) { return s_JoystickData[joystickId].isConnected(); }
		
		// Getters
		static bool getButton(int joystickId, int buttonCode);
		static bool getButtonDown(int joystickId, int buttonCode);
		static inline glm::vec2& getLeftStick(int joystickId) { return s_JoystickData[joystickId].getLeftStick(); }
		static inline glm::vec2& getRightStick(int joystickId) { return s_JoystickData[joystickId].getRightStick(); }
		static inline glm::vec2& getTriggers(int joystickId) { return s_JoystickData[joystickId].getTriggers(); }

	private:
		static JoystickInputData s_JoystickData[MAX_JOYSTICKS];
	};
}
