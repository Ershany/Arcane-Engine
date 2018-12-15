#include "pch.h"
#include "Time.h"

namespace arcane {

	Time::Time() {
		lastFrame = glfwGetTime();
		delta = 0;
	}

	void Time::update() {
		delta = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();
	}

}
