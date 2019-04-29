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

		void update();
		void joystickConnectionCallback(int joystick, int event);
		
		bool GetButton(int buttonCode); // Get the state of a certain button 
		//Get first button press
	private:
		static JoystickInputData s_JoystickData[MAX_JOYSTICKS]; // We can use a map if we don't want to create 16 values

	};
}
