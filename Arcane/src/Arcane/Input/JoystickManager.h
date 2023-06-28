#pragma once
#ifndef JOYSTICKMANAGER_H
#define JOYSTICKMANAGER_H

#ifndef JOYSTICKINPUTDATA_H
#include <Arcane/Input/JoystickInputData.h>
#endif

namespace Arcane
{
#define MAX_JOYSTICKS 16

	class JoystickManager {
	public:
		JoystickManager();
		~JoystickManager();
	
		void Update();

		void JoystickConnectionCallback(int joystick, int event);
		
		// Getters
		static JoystickInputData* GetJoystickInfo(int joystick);
		static inline bool IsConnected(int joystickId) { return s_JoystickData[joystickId].IsConnected(); }
		static bool GetButton(int joystickId, int buttonCode);
		static bool GetButtonDown(int joystickId, int buttonCode);
		static inline glm::vec2& GetLeftStick(int joystickId) { return s_JoystickData[joystickId].GetLeftStick(); }
		static inline glm::vec2& GetRightStick(int joystickId) { return s_JoystickData[joystickId].GetRightStick(); }
		static inline glm::vec2& GetTriggers(int joystickId) { return s_JoystickData[joystickId].GetTriggers(); }
	private:
		static JoystickInputData s_JoystickData[MAX_JOYSTICKS];
	};
}
#endif
