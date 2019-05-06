#pragma once

#include <input/JoystickInputData.h>

namespace arcane {

#define MAX_JOYSTICKS 16

	class JoystickManager {
	public:
		JoystickManager();
		~JoystickManager();
	
		void update();

		void joystickConnectionCallback(int joystick, int event);
		
		// Getters
		static JoystickInputData* getJoystickInfo(int joystick);
		static inline bool IsConnected(int joystickId) { return s_JoystickData[joystickId].isConnected(); }
		static bool getButton(int joystickId, int buttonCode);
		static bool getButtonDown(int joystickId, int buttonCode);
		static inline glm::vec2& getLeftStick(int joystickId) { return s_JoystickData[joystickId].getLeftStick(); }
		static inline glm::vec2& getRightStick(int joystickId) { return s_JoystickData[joystickId].getRightStick(); }
		static inline glm::vec2& getTriggers(int joystickId) { return s_JoystickData[joystickId].getTriggers(); }
	private:
		static JoystickInputData s_JoystickData[MAX_JOYSTICKS];
	};
}
